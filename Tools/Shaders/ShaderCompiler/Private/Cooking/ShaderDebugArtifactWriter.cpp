#include "PCH.h"

#include "Cooking/ShaderDebugArtifactWriter.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "Core/Public/Json/JsonWriter.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Cooking/ShaderCookContext.h"
#include "RHI/Public/Shaders/GlobalShaderMap.h"

#include <Windows.h>
#include <chrono>
#include <format>
#include <algorithm>
#include <utility>

ShaderDebugArtifactWriter::ShaderDebugArtifactWriter(const std::filesystem::path& rootDirectory)
{
	std::error_code error;
	const auto absoluteRoot = std::filesystem::absolute(rootDirectory);
	std::filesystem::create_directories(absoluteRoot, error);
	if (error)
	{
		throw Diagnostics::Error("Cannot create the shader debug export root.");
	}

	const auto cookId = std::chrono::steady_clock::now().time_since_epoch().count();
	const std::string directoryName = std::format("cook-{}", cookId);
	m_directory = absoluteRoot / directoryName;
	if (!std::filesystem::create_directory(m_directory, error) || error)
	{
		throw Diagnostics::Error("Shader debug export requires a fresh output directory.");
	}
}

std::string ShaderDebugArtifactWriter::HashFile(const std::filesystem::path& path)
{
	Hash::Sha256Digest digest;
	std::string error;
	if (!Hash::TrySha256File(path, digest, error))
	{
		throw Diagnostics::Error(error);
	}
	return Hash::Sha256ToHex(digest);
}

std::string ShaderDebugArtifactWriter::HashBytes(std::span<const std::byte> bytes)
{
	std::string hash;
	std::string error;
	if (!Hash::TrySha256Hex(bytes, hash, error))
	{
		throw Diagnostics::Error(error);
	}
	return hash;
}

std::string ShaderDebugArtifactWriter::WriteBytes(const std::filesystem::path& relativePath, std::span<const std::byte> contents)
{
	const std::string hash = HashBytes(contents);
	const std::string relativeName = relativePath.generic_string();
	Json::ObjectWriter file;
	file.WriteString("path", relativeName);
	file.WriteString("sha256", hash);
	file.WriteUInt64("bytes", contents.size());
	const std::string receipt = file.Finish();
	const auto [entry, inserted] = m_files.emplace(relativeName, receipt);
	if (!inserted)
	{
		if (entry->second != receipt)
		{
			throw Diagnostics::Error("Shader debug export contains conflicting bytes for the same source or artifact.");
		}
		return hash;
	}

	const std::filesystem::path outputPath = m_directory / relativePath;
	std::string fileError;
	if (!Files::TryWriteAllBytes(outputPath, contents, fileError))
	{
		throw Diagnostics::Error(fileError);
	}
	return hash;
}

void ShaderDebugArtifactWriter::WriteText(const std::filesystem::path& path, std::string_view contents)
{
	WriteBytes(path, std::as_bytes(std::span(contents.data(), contents.size())));
}

void ShaderDebugArtifactWriter::RecordCompiler(std::string_view backendName)
{
	if (backendName == "dxc")
	{
		RecordCompilerModule(L"dxcompiler.dll");
	}
	else if (backendName == "slang")
	{
		RecordCompilerModule(L"slang.dll");
		RecordCompilerModule(L"slang-compiler.dll");
	}
	else
	{
		throw Diagnostics::Error("Shader debug export has no compiler identity route for this backend.");
	}
	RecordCompilerModule(L"SPIRV-Tools-shared.dll");
}

void ShaderDebugArtifactWriter::RecordCompilerModule(const wchar_t* name)
{
	const std::string key = std::filesystem::path(name).generic_string();
	if (m_compilerModules.contains(key))
	{
		return;
	}
	const HMODULE module = GetModuleHandleW(name);
	std::wstring path(32768u, L'\0');
	const DWORD length = module != nullptr ? GetModuleFileNameW(module, path.data(), static_cast<DWORD>(path.size())) : 0u;
	if (length == 0u || length >= path.size())
	{
		throw Diagnostics::Error("Cannot identify the loaded shader compiler module '" + key + "'.");
	}
	path.resize(length);
	const std::filesystem::path modulePath(path);
	std::string moduleSha256 = HashFile(modulePath);
	m_compilerModules.emplace(key, std::move(moduleSha256));
}

void ShaderDebugArtifactWriter::Write(const ShaderCompileRequest& request, const CookedStageBuild& compiledStage, const ShaderDebugArtifactSet& debugArtifacts)
{
	if (!request.EnableDebugInfo || request.SourceFiles.empty() || compiledStage.bytecode.empty() || debugArtifacts.CompileArguments.empty()
	    || (compiledStage.format == ShaderBinaryFormat::Dxil && (debugArtifacts.Symbols.empty() || debugArtifacts.SymbolName.empty())))
	{
		throw Diagnostics::Error("Shader debug export is missing requested code, source or compiler debug data.");
	}

	RecordCompiler(compiledStage.backendName);
	const std::filesystem::path bundleDirectory = BuildBundleDirectoryName(request, compiledStage);
	const std::filesystem::path codePath = bundleDirectory / (compiledStage.format == ShaderBinaryFormat::SpirV ? "shader.spv" : "shader.dxil");
	const std::string codeSha256 = WriteBytes(codePath, std::as_bytes(std::span(compiledStage.bytecode)));

	for (const ShaderSourceFile& source : request.SourceFiles)
	{
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
		VerifyCapturedSource(request, source);
#endif
		const std::filesystem::path sourcePath = std::filesystem::path("sources") / std::filesystem::path(source.VirtualPath).relative_path();
		WriteText(sourcePath, source.Contents);
	}

	WriteText(bundleDirectory / "compiler-input.hlsl", request.SourceCode);
	WriteCompilerOutputs(bundleDirectory, debugArtifacts, compiledStage);
	const std::filesystem::path requestPath = bundleDirectory / "compile-request.json";
	const std::string compileRequestJson = BuildCompileRequestJson(request, compiledStage, codeSha256, debugArtifacts);
	const std::string definesJson = Json::WriteStringArray(request.Defines);
	WriteText(requestPath, compileRequestJson);
	WriteText(bundleDirectory / "defines.json", definesJson);
	m_jobs.push_back(requestPath.generic_string());
}

void ShaderDebugArtifactWriter::WriteCompilerOutputs(const std::filesystem::path& bundleDirectory, const ShaderDebugArtifactSet& debugArtifacts, const CookedStageBuild& compiledStage)
{
	WriteText(bundleDirectory / "preprocessed-source.hlsl", debugArtifacts.PreprocessedSource);
	const std::string reflectionJson = BuildReflectionJson(compiledStage.reflection);
	WriteText(bundleDirectory / "reflection.json", reflectionJson);
	WriteText(bundleDirectory / "parameter-struct-match.json", debugArtifacts.ParameterMatchReportJson);
	if (!debugArtifacts.Disassembly.empty())
	{
		WriteText(bundleDirectory / "disassembly.txt", debugArtifacts.Disassembly);
	}
	WriteText(bundleDirectory / "compiler-stderr.txt", debugArtifacts.CompilerOutput);
	const std::string compileArgumentsJson = Json::WriteStringArray(debugArtifacts.CompileArguments);
	WriteText(bundleDirectory / "compile-args.json", compileArgumentsJson);

	if (!debugArtifacts.Symbols.empty())
	{
		const std::filesystem::path symbolName = std::filesystem::path(debugArtifacts.SymbolName).filename();
		const std::string symbolFileName = symbolName.generic_string();
		const std::string safeSymbolFileName = Paths::MakeSafePathComponent(symbolFileName);
		if (symbolName.extension() != ".pdb" || symbolFileName != safeSymbolFileName)
		{
			throw Diagnostics::Error("Shader compiler returned an unsafe PDB lookup name.");
		}
		WriteBytes(bundleDirectory / symbolName, std::as_bytes(std::span(debugArtifacts.Symbols)));
	}
	if (!debugArtifacts.CompilerLookupKey.empty())
	{
		WriteBytes(bundleDirectory / "compiler-lookup-key.bin", std::as_bytes(std::span(debugArtifacts.CompilerLookupKey)));
	}
}

std::string ShaderDebugArtifactWriter::BuildBundleDirectoryName(const ShaderCompileRequest& request, const CookedStageBuild& compiledStage)
{
	const std::string shaderId = Paths::MakeSafePathComponent(request.ShaderTypeName);
	const std::string shaderTypeId = Formatting::FormatHexUInt64(request.ShaderType);
	const std::string backendName = Paths::MakeSafePathComponent(compiledStage.backendName);
	const std::string targetName = Paths::MakeSafePathComponent(GetShaderTargetName(request.Target));
	const std::string compileInputHash = Formatting::FormatHexUInt64(compiledStage.compileInputHash);
	return std::format("{}__{}__{}__{}__{}", shaderId, shaderTypeId, backendName, targetName, compileInputHash);
}

Files::FilePublication ShaderDebugArtifactWriter::StageManifest(
    const ShaderCookPipelinePlan& plan,
    std::span<const ShaderCompileResult> compileResults,
    const GlobalShaderMap& map,
    const CookedShaderLibrary& library,
    const std::filesystem::path& mapPath,
    const std::filesystem::path& libraryPath)
{
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	RecordSourceCandidate();
#endif
	WriteCompiledJobs(plan, compileResults, map, library);
	const std::filesystem::path manifest = m_directory / "shader-export.json";
	const std::filesystem::path stagedManifest = Files::BuildTemporaryPath(manifest, ".pending");
	const std::string manifestJson = BuildManifestJson(plan, mapPath, libraryPath);
	std::string error;
	if (!Files::TryWriteAllText(stagedManifest, manifestJson, error))
	{
		throw Diagnostics::Error(error);
	}
	return {.StagedPath = stagedManifest, .PublishedPath = manifest};
}

void ShaderDebugArtifactWriter::WriteCompiledJobs(
    const ShaderCookPipelinePlan& plan,
    std::span<const ShaderCompileResult> compileResults,
    const GlobalShaderMap& map,
    const CookedShaderLibrary& library)
{
	if (plan.jobs.size() != compileResults.size())
	{
		throw Diagnostics::Error("Shader debug export does not match the compiled job set.");
	}
	for (std::size_t index = 0; index < plan.jobs.size(); ++index)
	{
		const ShaderCompileJob& job = plan.jobs[index];
		const ShaderCompileResult& result = compileResults[index];
		const auto& products = plan.shaderOutputs[job.ShaderIndex];
		const auto product = std::ranges::find(products, job.Request.Target, &ShaderCookProduct::target);
		const GlobalShaderMapEntry* entry = map.Find(job.Request.ShaderType, job.Request.Target);
		if (product == products.end() || entry == nullptr || entry->CompileInputHash != job.InputHash || result.InputHash != job.InputHash || result.ShaderType != job.Request.ShaderType
		    || result.Target != job.Request.Target)
		{
			throw Diagnostics::Error("Shader debug export does not match its staged runtime entry.");
		}
		const CookedShaderCodeRecord* record = library.Find(entry->CodeHash);
		if (record == nullptr)
		{
			throw Diagnostics::Error("Shader debug export is missing its staged runtime code.");
		}
		const ShaderBytecode code = library.GetBytecode(*record);
		const auto* bytes = static_cast<const std::uint8_t*>(code.Data);
		if (code.Size != product->compiled.bytecode.size() || !std::equal(product->compiled.bytecode.begin(), product->compiled.bytecode.end(), bytes))
		{
			throw Diagnostics::Error("Shader debug export bytes differ from the staged runtime code.");
		}
		Write(job.Request, product->compiled, result.DebugArtifacts);
	}
}
