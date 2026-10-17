#pragma once

#include "Compiler/ShaderCompileRequest.h"
#include "Cooking/CookedStageBuild.h"
#include "ShaderDebugArtifactSet.h"
#include "Core/Public/Files/FileUtils.h"

#include <string_view>
#include <cstddef>
#include <filesystem>
#include <string>
#include <map>
#include <span>
#include <vector>

struct ShaderCookPipelinePlan;
struct ShaderCompileResult;
class GlobalShaderMap;
class CookedShaderLibrary;

class ShaderDebugArtifactWriter final
{
public:
	explicit ShaderDebugArtifactWriter(const std::filesystem::path& rootDirectory);
	Files::FilePublication StageManifest(
	    const ShaderCookPipelinePlan& plan,
	    std::span<const ShaderCompileResult> compileResults,
	    const GlobalShaderMap& map,
	    const CookedShaderLibrary& library,
	    const std::filesystem::path& mapPath,
	    const std::filesystem::path& libraryPath);

private:
	void WriteCompiledJobs(const ShaderCookPipelinePlan& plan, std::span<const ShaderCompileResult> compileResults, const GlobalShaderMap& map, const CookedShaderLibrary& library);
	std::string BuildManifestJson(const ShaderCookPipelinePlan& plan, const std::filesystem::path& mapPath, const std::filesystem::path& libraryPath) const;
	void Write(const ShaderCompileRequest& request, const CookedStageBuild& compiledStage, const ShaderDebugArtifactSet& debugArtifacts);
	std::string WriteBytes(const std::filesystem::path& relativePath, std::span<const std::byte> contents);
	void WriteText(const std::filesystem::path& relativePath, std::string_view contents);
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	void RecordSourceCandidate();
	void VerifyCapturedSource(const ShaderCompileRequest& request, const ShaderSourceFile& source) const;
#endif
	void RecordCompiler(std::string_view backendName);
	void RecordCompilerModule(const wchar_t* name);
	void WriteCompilerOutputs(const std::filesystem::path& bundleDirectory, const ShaderDebugArtifactSet& debugArtifacts, const CookedStageBuild& compiledStage);
	static std::string BuildBundleDirectoryName(const ShaderCompileRequest& request, const CookedStageBuild& compiledStage);
	static std::string BuildCompileRequestJson(const ShaderCompileRequest& request, const CookedStageBuild& compiledStage, std::string_view codeSha256, const ShaderDebugArtifactSet& debugArtifacts);
	static std::string BuildReflectionJson(const ShaderReflection& reflection);
	static std::string HashFile(const std::filesystem::path& path);
	static std::string HashBytes(std::span<const std::byte> bytes);

	std::filesystem::path m_directory;
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	std::map<std::wstring, std::string> m_sourceFileHashes;
#endif
	std::map<std::string, std::string> m_files;
	std::map<std::string, std::string> m_compilerModules;
	std::vector<std::string> m_jobs;
};
