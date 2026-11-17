#include "PCH.h"

#include "Cooking/Publication/ShaderDebugArtifactWriter.h"

#include "Cooking/Planning/ShaderCookPipelinePlan.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Formatting/HexFormat.h"

#include <nlohmann/json.hpp>

std::string ShaderDebugArtifactWriter::BuildCompileRequestJson(
    const ShaderCompileRequest& request,
    const CookedStageBuild& compiledStage,
    std::string_view codeSha256,
    const ShaderDebugArtifactSet& debugArtifacts)
{
	const std::string compilerInputSha256 = HashBytes(std::as_bytes(std::span(request.SourceCode)));
	const std::string symbolName = std::filesystem::path(debugArtifacts.SymbolName).filename().generic_string();
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
	    {"optimized", request.bEnableOptimizations},
	    {"symbolName", symbolName},
	    {"compilerLookupKeyKind", nullptr},
	    {"debugInformation", compiledStage.format == ShaderBinaryFormat::Dxil ? "full-pdb" : "embedded-spirv"},
	    {"sourceMapping", "unqualified"},
	    {"sources", nlohmann::json::array()}};
	if (!debugArtifacts.CompilerLookupKey.empty())
	{
		document["compilerLookupKeyKind"] = compiledStage.backendName == "dxc" ? "DxcShaderHash" : "SlangEntryPointCacheKey";
	}
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
