#include "PCH.h"

#include "Cooking/ShaderDebugArtifactWriter.h"

#include "Cooking/ShaderCookContext.h"
#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Json/JsonWriter.h"

#include <format>
#include <sstream>

std::string ShaderDebugArtifactWriter::BuildCompileRequestJson(
    const ShaderCompileRequest& request,
    const CookedStageBuild& compiledStage,
    std::string_view codeSha256,
    const ShaderDebugArtifactSet& debugArtifacts)
{
	const std::string compilerInputSha256 = HashBytes(std::as_bytes(std::span(request.SourceCode)));
	const std::string symbolName = std::filesystem::path(debugArtifacts.SymbolName).filename().generic_string();
	Json::ObjectWriter writer;
	writer.WriteHexUInt64("shaderTypeId", request.ShaderType);
	writer.WriteString("shaderType", request.ShaderTypeName);
	writer.WriteString("sourcePath", request.VirtualSourcePath);
	writer.WriteString("entryPoint", request.EntryPoint);
	writer.WriteString("stage", GetShaderStagePrefix(request.Stage));
	writer.WriteString("target", GetShaderTargetName(request.Target));
	writer.WriteString("backend", compiledStage.backendName);
	writer.WriteUInt64("backendVersion", compiledStage.backendVersion);
	writer.WriteString("format", compiledStage.format == ShaderBinaryFormat::SpirV ? "SpirV" : "Dxil");
	writer.WriteHexUInt64("sourceHash", compiledStage.sourceHash);
	writer.WriteHexUInt64("includeClosureHash", compiledStage.includeClosureHash);
	writer.WriteHexUInt64("requestHash", compiledStage.requestHash);
	writer.WriteHexUInt64("compileInputHash", compiledStage.compileInputHash);
	writer.WriteString("codeSha256", codeSha256);
	writer.WriteString("compilerInputSha256", compilerInputSha256);
	writer.WriteRaw("optimized", request.EnableOptimizations ? "true" : "false");
	writer.WriteString("symbolName", symbolName);
	if (debugArtifacts.CompilerLookupKey.empty())
	{
		writer.WriteRaw("compilerLookupKeyKind", "null");
	}
	else
	{
		writer.WriteString("compilerLookupKeyKind", compiledStage.backendName == "dxc" ? "DxcShaderHash" : "SlangEntryPointCacheKey");
	}
	writer.WriteString("debugInformation", compiledStage.format == ShaderBinaryFormat::Dxil ? "full-pdb" : "embedded-spirv");
	writer.WriteString("sourceMapping", "unqualified");

	std::vector<std::string> sources;
	for (const ShaderSourceFile& source : request.SourceFiles)
	{
		const std::filesystem::path sourcePath = std::filesystem::path("sources") / std::filesystem::path(source.VirtualPath).relative_path();
		sources.push_back(sourcePath.generic_string());
	}
	const std::string sourcesJson = Json::WriteStringArray(sources);
	writer.WriteRaw("sources", sourcesJson);
	return writer.Finish();
}

std::string ShaderDebugArtifactWriter::BuildManifestJson(const ShaderCookPipelinePlan& plan, const std::filesystem::path& mapPath, const std::filesystem::path& libraryPath) const
{
	const std::string mapSha256 = HashFile(mapPath);
	const std::string librarySha256 = HashFile(libraryPath);
	const std::filesystem::path executablePath = Filesystem::GetExecutablePath();
	const std::string compilerExecutableSha256 = HashFile(executablePath);
	Json::ObjectWriter writer;
	writer.WriteString("scope", plan.shaders.size() == plan.registeredShaderTypes.size() ? "registered-catalog" : "compiled-selection");
	writer.WriteString("globalShaderMapSha256", mapSha256);
	writer.WriteString("cookedShaderLibrarySha256", librarySha256);
	writer.WriteString("shaderCompilerExecutableSha256", compilerExecutableSha256);

	Json::ObjectWriter compilers;
	for (const auto& [name, hash] : m_compilerModules)
	{
		compilers.WriteString(name, hash);
	}
	const std::string compilerModulesJson = compilers.Finish();
	const std::string jobsJson = Json::WriteStringArray(m_jobs);
	writer.WriteRaw("compilerModules", compilerModulesJson);
	writer.WriteRaw("jobs", jobsJson);

	std::string files = "[";
	for (const auto& file : m_files)
	{
		if (files.size() > 1)
		{
			files += ',';
		}
		files += file.second;
	}
	files += ']';
	writer.WriteRaw("files", files);
	writer.WriteRaw("nativeShaderIdentifiers", "null");
	writer.WriteRaw("nvidiaDebugInfoIdentifiers", "null");
	writer.WriteRaw("cpuPdbIdentity", "null");
	return writer.Finish();
}

std::string ShaderDebugArtifactWriter::BuildReflectionJson(const ShaderReflection& reflection)
{
	std::ostringstream stream;
	stream << "{\n";
	stream << "  \"threadGroupSize\": [" << reflection.ThreadGroupSize[0] << ", " << reflection.ThreadGroupSize[1] << ", " << reflection.ThreadGroupSize[2] << "],\n";
	stream << "  \"entryFlags\": " << reflection.EntryFlags << ",\n";
	stream << "  \"waveSize\": " << reflection.WaveSize << ",\n";
	stream << "  \"bindings\": [\n";
	for (std::size_t index = 0; index < reflection.Bindings.size(); ++index)
	{
		const ShaderReflectionResourceBinding& binding = reflection.Bindings[index];
		stream << std::format(
		    "    {{ \"name\": {}, \"set\": {}, \"slot\": {}, \"arrayCount\": {}, \"sizeInBytes\": {}, "
		    "\"readOnly\": {}, \"constantBufferIndex\": {} }}",
		    Json::QuoteString(binding.Name),
		    binding.Set,
		    binding.Slot,
		    binding.ArrayCount,
		    binding.SizeInBytes,
		    binding.IsReadOnly ? "true" : "false",
		    binding.ConstantBufferIndex);
		if (index + 1 < reflection.Bindings.size())
		{
			stream << ',';
		}
		stream << "\n";
	}
	stream << "  ],\n";
	stream << "  \"constantBuffers\": [\n";
	for (std::size_t index = 0; index < reflection.ConstantBuffers.size(); ++index)
	{
		const ShaderReflectionConstantBuffer& cb = reflection.ConstantBuffers[index];
		stream << std::format("    {{ \"name\": {}, \"sizeInBytes\": {}, \"memberCount\": {} }}", Json::QuoteString(cb.Name), cb.SizeInBytes, cb.Members.size());
		if (index + 1 < reflection.ConstantBuffers.size())
		{
			stream << ',';
		}
		stream << "\n";
	}
	stream << "  ],\n";
	stream << "  \"inputElements\": [\n";
	for (std::size_t index = 0; index < reflection.InputElements.size(); ++index)
	{
		const ShaderReflectionInputElement& input = reflection.InputElements[index];
		stream << std::format(
		    "    {{ \"semantic\": {}, \"semanticIndex\": {}, \"location\": {}, \"componentCount\": {} }}",
		    Json::QuoteString(input.Semantic),
		    input.SemanticIndex,
		    input.Location,
		    input.ComponentCount);
		if (index + 1 < reflection.InputElements.size())
		{
			stream << ',';
		}
		stream << "\n";
	}
	stream << "  ],\n";
	stream << "  \"pushConstants\": [\n";
	for (std::size_t index = 0; index < reflection.PushConstants.size(); ++index)
	{
		const ShaderReflectionPushConstantRange& range = reflection.PushConstants[index];
		stream << std::format("    {{ \"offsetInBytes\": {}, \"sizeInBytes\": {}, \"visibilityMask\": {} }}", range.OffsetInBytes, range.SizeInBytes, static_cast<unsigned>(range.VisibilityMask));
		if (index + 1 < reflection.PushConstants.size())
		{
			stream << ',';
		}
		stream << "\n";
	}
	stream << "  ],\n";
	stream << "  \"specializationConstants\": [\n";
	for (std::size_t index = 0; index < reflection.SpecializationConstants.size(); ++index)
	{
		const ShaderReflectionSpecializationConstant& spec = reflection.SpecializationConstants[index];
		stream << std::format("    {{ \"name\": {}, \"constantId\": {}, \"defaultValueBits\": {} }}", Json::QuoteString(spec.Name), spec.ConstantId, spec.DefaultValueBits);
		if (index + 1 < reflection.SpecializationConstants.size())
		{
			stream << ',';
		}
		stream << "\n";
	}
	stream << "  ]\n";
	stream << "}\n";
	return stream.str();
}
