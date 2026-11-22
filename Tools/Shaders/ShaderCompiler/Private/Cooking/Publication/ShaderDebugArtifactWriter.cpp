#include "PCH.h"
#include "Core/Public/Hash/FileHash.h"

#include "Cooking/Publication/ShaderDebugArtifactWriter.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Cooking/Planning/ShaderCookPipelinePlan.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "RHI/Public/Shaders/GlobalShaderMap.h"

#include "Compiler/ShaderCompilerToolchain.h"
#include <chrono>
#include <format>
#include <algorithm>
#include <utility>
#include <nlohmann/json.hpp>

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
	const auto [entry, bInserted] = m_files.emplace(relativeName, FileReceipt{hash, contents.size()});
	if (!bInserted)
	{
		if (entry->second.Sha256 != hash || entry->second.Bytes != contents.size())
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

std::string ShaderDebugArtifactWriter::WriteText(const std::filesystem::path& path, std::string_view contents)
{
	return WriteBytes(path, std::as_bytes(std::span(contents.data(), contents.size())));
}

void ShaderDebugArtifactWriter::RecordCompiler(std::uint64_t version)
{
	if (!m_compilerModules.empty())
	{
		return;
	}
	const ShaderCompilerToolchain toolchain = ReadShaderCompilerToolchain();
	if (toolchain.Version != version)
	{
		throw Diagnostics::Error("Shader compiler inputs changed before debug publication.");
	}
	for (const ShaderCompilerInputFile& input : toolchain.Files)
	{
		m_compilerModules.emplace(input.Name, input.Sha256);
	}
}

void ShaderDebugArtifactWriter::Write(const ShaderCompileRequest& request, const CookedStageBuild& compiledStage, const ShaderDebugArtifactSet& debugArtifacts)
{
	const bool bSourceReady = request.bEnableDebugInfo && !request.SourceFiles.empty();
	const bool bCodeReady = !compiledStage.bytecode.empty() && debugArtifacts.Invocation != nullptr;
	const bool bCoreReady = bSourceReady && bCodeReady;
	const bool bSymbolsReady = !debugArtifacts.Symbols.empty() && !debugArtifacts.SymbolName.empty();
	const bool bFormatReady = compiledStage.format != ShaderBinaryFormat::Dxil || bSymbolsReady;
	const bool bInputsReady = bCoreReady && bFormatReady;

	if (!bInputsReady)
	{
		throw Diagnostics::Error("Shader debug export is missing requested code, source or compiler debug data.");
	}

	RecordCompiler(compiledStage.backendVersion);
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

	const std::string compilerInputSha256 = WriteText(std::filesystem::path("sources") / debugArtifacts.CompilerInputPath, request.SourceCode);
	WriteCompilerOutputs(bundleDirectory, debugArtifacts, compiledStage);
	const nlohmann::json parameterReport{{"status", request.ParameterStruct.has_value() ? "matched" : "skipped"}};
	WriteText(bundleDirectory / "parameter-struct-match.json", parameterReport.dump());
	const std::filesystem::path requestPath = bundleDirectory / "compile-request.json";
	const std::string compileRequestJson = BuildCompileRequestJson(request, compiledStage, codeSha256, compilerInputSha256, debugArtifacts);
	WriteText(requestPath, compileRequestJson);
	WriteText(bundleDirectory / "defines.json", nlohmann::json(request.Defines).dump());
	m_jobs.push_back(requestPath.generic_string());
}

void ShaderDebugArtifactWriter::WriteCompilerOutputs(const std::filesystem::path& bundleDirectory, const ShaderDebugArtifactSet& debugArtifacts, const CookedStageBuild& compiledStage)
{
	WriteText(bundleDirectory / "preprocessed-source.hlsl", debugArtifacts.PreprocessedSource);
	const std::string reflectionJson = BuildReflectionJson(compiledStage.reflection);
	WriteText(bundleDirectory / "reflection.json", reflectionJson);
	if (!debugArtifacts.Disassembly.empty())
	{
		WriteText(bundleDirectory / "disassembly.txt", debugArtifacts.Disassembly);
	}
	const ShaderCompileInvocation& invocation = *debugArtifacts.Invocation;
	WriteText(bundleDirectory / "compile-args.json", nlohmann::json(invocation.Arguments).dump());
	WriteText(std::filesystem::path("sources") / invocation.ConfigurationPath, invocation.Configuration);
	std::filesystem::path compilerOutputPath = invocation.ConfigurationPath;
	compilerOutputPath.replace_extension(".log");
	WriteText(std::filesystem::path("sources") / compilerOutputPath, invocation.CompilerOutput);
	if (debugArtifacts.PreprocessingInvocation)
	{
		const ShaderCompileInvocation& preprocessing = *debugArtifacts.PreprocessingInvocation;
		WriteText(bundleDirectory / "preprocess-args.json", nlohmann::json(preprocessing.Arguments).dump());
		WriteText(std::filesystem::path("sources") / preprocessing.ConfigurationPath, preprocessing.Configuration);
		std::filesystem::path preprocessingOutputPath = preprocessing.ConfigurationPath;
		preprocessingOutputPath.replace_extension(".log");
		WriteText(std::filesystem::path("sources") / preprocessingOutputPath, preprocessing.CompilerOutput);
	}

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
	try
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
	catch (const nlohmann::json::exception&)
	{
		throw Diagnostics::Error("Shader debug export contains invalid UTF-8 or an unsupported JSON value.");
	}
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
		const bool bProductMissing = product == products.end() || entry == nullptr;
		const bool bEntryMismatch = bProductMissing || (entry->CompileInputHash != job.InputHash);
		const bool bCompileMismatch = bEntryMismatch || (result.InputHash != job.InputHash);
		const bool bShaderMismatch = bCompileMismatch || (result.ShaderType != job.Request.ShaderType);
		const bool bExportMismatch = bShaderMismatch || (result.Target != job.Request.Target);

		if (bExportMismatch)
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
		const bool bDiagnosticBytecodeMismatch = code.Size != product->compiled.bytecode.size() || !std::equal(product->compiled.bytecode.begin(), product->compiled.bytecode.end(), bytes);

		if (bDiagnosticBytecodeMismatch)
		{
			throw Diagnostics::Error("Shader debug export bytes differ from the staged runtime code.");
		}
		Write(job.Request, product->compiled, result.DebugArtifacts);
	}
}

std::string ShaderDebugArtifactWriter::BuildCompileRequestJson(
    const ShaderCompileRequest& request,
    const CookedStageBuild& compiledStage,
    std::string_view codeSha256,
    std::string_view compilerInputSha256,
    const ShaderDebugArtifactSet& debugArtifacts)
{
	const std::string symbolName = std::filesystem::path(debugArtifacts.SymbolName).filename().generic_string();
	std::filesystem::path compilerOutputPath = debugArtifacts.Invocation->ConfigurationPath;
	compilerOutputPath.replace_extension(".log");

	nlohmann::json document{
	    {"shaderTypeId", Formatting::FormatHexUInt64(request.ShaderType)},
	    {"shaderType", request.ShaderTypeName},
	    {"sourcePath", request.VirtualSourcePath},
	    {"entryPoint", request.EntryPoint},
	    {"stage", GetShaderStagePrefix(request.Stage)},
	    {"target", GetShaderTargetName(request.Target)},
	    {"backend", compiledStage.backendName},
	    {"backendVersion", compiledStage.backendVersion},
	    {"format", compiledStage.format == ShaderBinaryFormat::SpirV ? "SpirV" : "Dxil"},
	    {"sourceHash", Formatting::FormatHexUInt64(compiledStage.sourceHash)},
	    {"includeClosureHash", Formatting::FormatHexUInt64(compiledStage.includeClosureHash)},
	    {"requestHash", Formatting::FormatHexUInt64(compiledStage.requestHash)},
	    {"compileInputHash", Formatting::FormatHexUInt64(compiledStage.compileInputHash)},
	    {"codeSha256", codeSha256},
	    {"compilerInputSha256", compilerInputSha256},
	    {"compilerInputPath", (std::filesystem::path("sources") / debugArtifacts.CompilerInputPath).generic_string()},
	    {"configurationPath", (std::filesystem::path("sources") / debugArtifacts.Invocation->ConfigurationPath).generic_string()},
	    {"compilerOutputPath", (std::filesystem::path("sources") / compilerOutputPath).generic_string()},
	    {"executionWorkingDirectory", debugArtifacts.Invocation->WorkingDirectory.generic_string()},
	    {"replayWorkingDirectory", "sources"},
	    {"replayCompilerOverrideRequired", true},
	    {"optimized", request.bEnableOptimizations},
	    {"symbolName", symbolName},
	    {"compilerLookupKeyKind", nullptr},
	    {"debugInformation", compiledStage.format == ShaderBinaryFormat::Dxil ? "full-pdb" : "embedded-spirv"},
	    {"sourceMapping", "unqualified"},
	    {"sources", nlohmann::json::array()}};

	for (const ShaderSourceFile& source : request.SourceFiles)
	{
		const std::filesystem::path sourcePath = std::filesystem::path("sources") / std::filesystem::path(source.VirtualPath).relative_path();
		document["sources"].push_back(sourcePath.generic_string());
	}
	return document.dump();
}

std::string ShaderDebugArtifactWriter::BuildManifestJson(const ShaderCookPipelinePlan& plan, const std::filesystem::path& mapPath, const std::filesystem::path& libraryPath) const
{
	nlohmann::json document{
	    {"scope", plan.shaders.size() == plan.registeredShaderTypes.size() ? "registered-catalog" : "compiled-selection"},
	    {"globalShaderMapSha256", HashFile(mapPath)},
	    {"cookedShaderLibrarySha256", HashFile(libraryPath)},
	    {"shaderCompilerExecutableSha256", HashFile(Filesystem::GetExecutablePath())},
	    {"compilerModules", m_compilerModules},
	    {"jobs", m_jobs},
	    {"files", nlohmann::json::array()},
	    {"nativeShaderIdentifiers", nullptr},
	    {"nvidiaDebugInfoIdentifiers", nullptr},
	    {"cpuPdbIdentity", nullptr}};

	for (const auto& [path, receipt] : m_files)
	{
		document["files"].push_back({{"path", path}, {"sha256", receipt.Sha256}, {"bytes", receipt.Bytes}});
	}
	return document.dump();
}

std::string ShaderDebugArtifactWriter::BuildReflectionJson(const ShaderReflection& reflection)
{
	nlohmann::json document{
	    {"threadGroupSize", reflection.ThreadGroupSize},
	    {"entryFlags", reflection.EntryFlags},
	    {"waveSize", reflection.WaveSize},
	    {"bindings", nlohmann::json::array()},
	    {"constantBuffers", nlohmann::json::array()},
	    {"inputElements", nlohmann::json::array()},
	    {"pushConstants", nlohmann::json::array()},
	    {"specializationConstants", nlohmann::json::array()}};

	for (const ShaderReflectionResourceBinding& binding : reflection.Bindings)
	{
		document["bindings"].push_back(
		    {{"name", binding.Name},
		        {"set", binding.Set},
		        {"slot", binding.Slot},
		        {"arrayCount", binding.ArrayCount},
		        {"sizeInBytes", binding.SizeInBytes},
		        {"readOnly", binding.bIsReadOnly},
		        {"constantBufferIndex", binding.ConstantBufferIndex}});
	}
	for (const ShaderReflectionConstantBuffer& buffer : reflection.ConstantBuffers)
	{
		document["constantBuffers"].push_back({{"name", buffer.Name}, {"sizeInBytes", buffer.SizeInBytes}, {"memberCount", buffer.Members.size()}});
	}
	for (const ShaderReflectionInputElement& input : reflection.InputElements)
	{
		document["inputElements"].push_back({{"semantic", input.Semantic}, {"semanticIndex", input.SemanticIndex}, {"location", input.Location}, {"componentCount", input.ComponentCount}});
	}
	for (const ShaderReflectionPushConstantRange& range : reflection.PushConstants)
	{
		document["pushConstants"].push_back({{"offsetInBytes", range.OffsetInBytes}, {"sizeInBytes", range.SizeInBytes}, {"visibilityMask", static_cast<unsigned>(range.VisibilityMask)}});
	}
	for (const ShaderReflectionSpecializationConstant& constant : reflection.SpecializationConstants)
	{
		document["specializationConstants"].push_back({{"name", constant.Name}, {"constantId", constant.ConstantId}, {"defaultValueBits", constant.DefaultValueBits}});
	}
	return document.dump();
}
