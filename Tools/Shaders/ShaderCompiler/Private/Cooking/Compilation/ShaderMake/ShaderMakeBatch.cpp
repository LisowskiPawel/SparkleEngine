#include "PCH.h"

#include "Cooking/Compilation/ShaderMake/ShaderMakeBatch.h"
#include "Cooking/Compilation/ShaderMake/ShaderMakeConfig.h"
#include "Cooking/Compilation/ShaderMake/ShaderMakeOutputs.h"
#include "Cooking/Compilation/ShaderMake/ShaderMakeProducer.h"
#include "Cooking/ShaderCookDiagnostics.h"
#include "Cooking/ShaderCookSettings.h"
#include "Compiler/ShaderCompilerToolchain.h"
#include "Compiler/ShaderSourceMountTable.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Process/ChildProcess.h"
#include "Core/Public/Strings/StringUtils.h"

#include <Windows.h>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <format>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <tuple>
#include <utility>

class ShaderMakeStaging final
{
public:
	ShaderMakeStaging();
	~ShaderMakeStaging();
	ShaderMakeStaging(const ShaderMakeStaging&) = delete;
	ShaderMakeStaging& operator=(const ShaderMakeStaging&) = delete;

	std::filesystem::path Directory;
	bool bCanRemove = true;
};

ShaderMakeStaging::ShaderMakeStaging()
{
	const std::filesystem::path parent = Filesystem::ResolveArtifactRootPath(Filesystem::GetWorkspaceRootPath()) / ".shader-make";
	std::filesystem::create_directories(parent);
	const DWORD attributes = GetFileAttributesW(parent.c_str());
	if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
	{
		throw Diagnostics::Error("ShaderMake staging requires an owned directory without a reparse point.");
	}
	const auto clock = std::chrono::steady_clock::now().time_since_epoch().count();
	Directory = parent / std::format("{:x}-{:x}", GetCurrentProcessId(), clock);
	if (!std::filesystem::create_directory(Directory))
	{
		throw Diagnostics::Error("ShaderMake requires a fresh invocation directory.");
	}
}

ShaderMakeStaging::~ShaderMakeStaging()
{
	if (!bCanRemove)
	{
		return;
	}
	try
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(Directory))
		{
			const DWORD attributes = GetFileAttributesW(entry.path().c_str());
			if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
			{
				return;
			}
		}
		std::error_code ignored;
		std::filesystem::remove_all(Directory, ignored);
	}
	catch (...)
	{
		// Preserve inaccessible staging rather than masking the compilation result.
	}
}

struct ShaderMakeGroup final
{
	std::string_view Backend;
	ShaderTarget Target = kDefaultShaderTarget;
	bool bCaptureDebugArtifacts = false;
	std::vector<ShaderMakeProducer> Producers;
};

static ShaderMakeProducer MakeProducer(const ShaderCompileJob& job, std::size_t index)
{
	const std::string name = std::format("p{:x}", index);
	const std::string suffix = job.Request.EntryPoint == "main" ? "" : "_" + job.Request.EntryPoint;
	const std::filesystem::path output = std::filesystem::path("out") / name;
	const bool bDxil = IsDxilTarget(job.Request.Target);
	const bool bSlang = job.BackendName == "slang";
	const bool bSlangDxil = bSlang && bDxil;
	const bool bContainer = bSlangDxil && job.Request.bCaptureDebugArtifacts;
	const std::string extension = bContainer ? "" : bDxil ? ".dxil" : ".spirv";
	const std::filesystem::path code = output / (name + suffix + extension);
	return {
		.Job = &job,
		.ResultIndex = index,
		.Name = name,
		.Input = std::filesystem::path("_generated") / (name + ".hlsl"),
		.Code = bContainer ? code / "children/0/0.dxil" : code,
		.Reflection = output / (bSlang ? "reflection.json" : "reflection.bin"),
		.Preprocessed = std::filesystem::path("preprocessed") / name / (name + suffix + ".hlsl"),
		.Symbols = bContainer ? code / "children/0/associated" : output};
}

static std::vector<ShaderMakeGroup> GroupProducers(std::span<const ShaderCompileJob> jobs, std::span<const std::size_t> indices)
{
	std::vector<ShaderMakeGroup> groups;
	for (std::size_t index = 0; index < indices.size(); ++index)
	{
		const ShaderCompileJob& job = jobs[indices[index]];
		const auto key = std::tie(job.BackendName, job.Request.Target, job.Request.bCaptureDebugArtifacts);
		const auto group = std::ranges::find_if(groups, [&key](const ShaderMakeGroup& candidate)
		{
			return std::tie(candidate.Backend, candidate.Target, candidate.bCaptureDebugArtifacts) == key;
		});
		if (group == groups.end())
		{
			groups.push_back({.Backend = job.BackendName, .Target = job.Request.Target, .bCaptureDebugArtifacts = job.Request.bCaptureDebugArtifacts});
			groups.back().Producers.push_back(MakeProducer(job, index));
		}
		else
		{
			group->Producers.push_back(MakeProducer(job, index));
		}
	}
	return groups;
}

static void WriteCompilerText(const std::filesystem::path& path, std::string_view text)
{
	std::string error;
	if (!Files::TryWriteAllText(path, text, error))
	{
		throw Diagnostics::Error(error);
	}
}

static void WriteCapturedSources(const std::filesystem::path& root, const ShaderMakeGroup& group)
{
	std::map<std::string, std::string_view> sources;
	std::map<std::string, std::string> casing;
	for (const ShaderMakeProducer& producer : group.Producers)
	{
		WriteCompilerText(root / producer.Input, producer.Job->Request.SourceCode);
		for (const ShaderSourceFile& file : producer.Job->Request.SourceFiles)
		{
			const std::string canonical = producer.Job->Request.SourceMounts.get().CanonicalizeVirtualPath(file.VirtualPath);
			const std::string name = canonical.substr(1);
			const auto [caseEntry, bCaseInserted] = casing.emplace(Strings::ToLowerCopy(name), name);
			if (!bCaseInserted && caseEntry->second != name)
			{
				throw Diagnostics::Error("Captured shader sources collide under the virtual-path case policy.");
			}
			const auto [source, bInserted] = sources.emplace(name, file.Contents);
			if (!bInserted && source->second != file.Contents)
			{
				throw Diagnostics::Error("Captured shader source changed between planned producers.");
			}
		}
	}
	for (const auto& [name, contents] : sources)
	{
		WriteCompilerText(root / std::filesystem::path(name), contents);
	}
}

static void VerifyToolchain(std::uint64_t expected)
{
	if (ReadShaderCompilerToolchain().Version != expected)
	{
		throw Diagnostics::Error("Shader compiler inputs changed after planning.");
	}
}

static std::shared_ptr<const ShaderCompileInvocation> RunShaderMake(
	ShaderMakeStaging& staging,
	const ShaderMakeGroup& group,
	std::size_t groupIndex,
	std::uint32_t workers,
	ShaderMakeOperation operation,
	std::string& diagnostics)
{
	const ShaderCompileJob& job = *group.Producers.front().Job;
	VerifyToolchain(job.BackendVersion);
	auto invocation = std::make_shared<ShaderCompileInvocation>();
	invocation->ConfigurationPath = std::filesystem::path("_requests") / std::format("group-{}{}.cfg", groupIndex, operation == ShaderMakeOperation::Preprocess ? "-preprocess" : "");
	invocation->Configuration = BuildShaderMakeConfiguration(group.Producers, operation);
	WriteCompilerText(staging.Directory / invocation->ConfigurationPath, invocation->Configuration);
	Process::ChildProcessRequest process;
	process.ExecutablePath = Filesystem::GetExecutableDirectory() / "ShaderMake.exe";
	const std::filesystem::path compiler = Filesystem::GetExecutableDirectory() / (group.Backend == "dxc" ? "dxc.exe" : "slangc.exe");
	const std::string compilerPath = compiler.lexically_relative(staging.Directory).generic_string();
	const bool bShellSafe = !compilerPath.empty() && std::ranges::all_of(compilerPath, [](unsigned char character)
	{
		return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z')
			|| (character >= '0' && character <= '9') || character == '/' || character == '.' || character == '_' || character == '-';
	});
	if (!bShellSafe)
	{
		throw Diagnostics::Error("ShaderMake requires a same-volume compiler path with ASCII artifact components.");
	}
	process.Arguments = BuildShaderMakeArguments(job, invocation->ConfigurationPath.generic_string(), compilerPath, workers, operation);
	process.WorkingDirectory = staging.Directory;
	invocation->Arguments.push_back(process.ExecutablePath.generic_string());
	invocation->Arguments.insert(invocation->Arguments.end(), process.Arguments.begin(), process.Arguments.end());
	staging.bCanRemove = false;
	const Process::ChildProcessResult result = Process::RunChildProcess(process);
	staging.bCanRemove = !result.bLaunched || result.FailureReason.empty();
	diagnostics += result.CapturedOutput;
	if (!result.Succeeded())
	{
		throw Diagnostics::Error(std::format("ShaderMake {} batch failed (exit {}): {}\n{}\nInvocation: {}",
			group.Backend, result.ExitCode, result.FailureReason, result.CapturedOutput, staging.Directory.string()));
	}
	VerifyToolchain(job.BackendVersion);
	return invocation;
}

std::vector<ShaderCompileResult> CompileShaderMakeProducers(
	const ShaderCookSettings& settings,
	std::span<const ShaderCompileJob> jobs,
	std::span<const std::size_t> producerIndices,
	std::size_t totalWork,
	const ToolWorkProgressCallback& progress)
{
	const std::uint32_t hardwareThreads = std::max(1u, std::thread::hardware_concurrency());
	const std::uint32_t workers = std::clamp(settings.maximumParallelCompiles, 1u, std::min(hardwareThreads, 8u));
	const std::vector<ShaderMakeGroup> groups = GroupProducers(jobs, producerIndices);
	std::vector<ShaderCompileResult> results(producerIndices.size());
	std::size_t completed = 0;
	for (std::size_t index = 0; index < groups.size(); ++index)
	{
		const ShaderMakeGroup& group = groups[index];
		ShaderMakeStaging staging;
		WriteCapturedSources(staging.Directory, group);
		std::string diagnostics;
		const auto invocation = RunShaderMake(staging, group, index, workers, ShaderMakeOperation::Compile, diagnostics);
		std::shared_ptr<const ShaderCompileInvocation> preprocessing;
		if (group.Backend == "dxc" && group.bCaptureDebugArtifacts)
		{
			preprocessing = RunShaderMake(staging, group, index, workers, ShaderMakeOperation::Preprocess, diagnostics);
		}
		for (const ShaderMakeProducer& producer : group.Producers)
		{
			try
			{
				ShaderCompileResult result = ReadShaderMakeOutput(staging.Directory, producer, invocation, diagnostics);
				result.DebugArtifacts.PreprocessingInvocation = preprocessing;
				results[producer.ResultIndex] = std::move(result);
			}
			catch (const std::exception& error)
			{
				throw Diagnostics::Error(std::format("Failed to ingest {} - {}", FormatShaderCompileJob(*producer.Job), error.what()));
			}
			if (progress)
			{
				progress({.Action = "Read compiled producer", .Item = producer.Job->Request.ShaderTypeName, .Completed = ++completed, .Total = totalWork});
			}
		}
	}
	return results;
}
