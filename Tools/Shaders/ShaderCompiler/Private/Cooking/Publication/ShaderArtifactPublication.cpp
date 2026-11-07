#include "PCH.h"

#include "Cooking/Publication/ShaderArtifactPublication.h"

#include "Cooking/Planning/Dependencies/ShaderDependencyManifest.h"
#include "Cooking/Publication/ShaderArtifactWriter.h"
#include "Cooking/Publication/ShaderDebugArtifactWriter.h"
#include "Cooking/ShaderCookSettings.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Paths/ProductFilePaths.h"
#include "RHI/Public/Shaders/GlobalShaderMap.h"

#include <utility>

void PrepareShaderProducts(ShaderCookPipelinePlan& plan, std::span<ShaderCompileResult> compileResults)
{
	for (std::size_t jobIndex = 0; jobIndex < plan.jobs.size(); ++jobIndex)
	{
		const ShaderCompileJob& job = plan.jobs[jobIndex];
		const ShaderCookDesc& shader = plan.shaders[job.ShaderIndex];
		ShaderCompileResult& compiled = compileResults[jobIndex];
		if (compiled.ShaderType != job.Request.ShaderType || compiled.Target != job.Request.Target || compiled.InputHash != job.InputHash)
		{
			throw Diagnostics::Error("Shader compile result does not match its immutable job identity.");
		}
		plan.shaderOutputs[job.ShaderIndex].push_back(
		    ShaderCookProduct{
		        .shaderTypeId = shader.shaderTypeId,
		        .target = compiled.Target,
		        .features = shader.features,
		        .rayTracing = shader.rayTracing,
		        .parameterLayout = shader.parameterLayout,
		        .bindingRemaps = job.Request.DescriptorBindingRemaps,
		        .compiled = std::move(compiled.Output)});
	}
}

ShaderCookOutput PublishShaderArtifacts(
    const ShaderCookPipelinePlan& plan,
    std::span<const ShaderCompileResult> compileResults,
    const ShaderCookSettings& settings,
    const std::filesystem::path& outputDirectory)
{
	const bool bReplaceCompleteCatalog = settings.shaderId.empty() && settings.changedVirtualPaths.empty();

	const std::filesystem::path debugDirectory = settings.debugArtifactDirectory.empty()
	    ? (Filesystem::ResolveArtifactRootPath(Filesystem::GetWorkspaceRootPath()) / "symbols" / "shaders" / Filesystem::GetProjectPath().filename())
	    : settings.debugArtifactDirectory;

	std::vector<std::filesystem::path> publicationRoots{outputDirectory};
	if (settings.bEnableDebugInfo || !settings.debugArtifactDirectory.empty())
	{
		publicationRoots.push_back(debugDirectory);
	}
	Files::FileSetPublisher publisher(publicationRoots);
	const std::filesystem::path& mapPath = Filesystem::GetGlobalShaderMapPath();
	const std::filesystem::path& libraryPath = Filesystem::GetCookedShaderLibraryPath();
	const std::filesystem::path stagedMapPath = Files::BuildTemporaryPath(mapPath, ".cook-generation");
	const std::filesystem::path stagedLibraryPath = Files::BuildTemporaryPath(libraryPath, ".cook-generation");
	const std::filesystem::path dependencyPath = ShaderDependencyManifest::GetPath(outputDirectory);
	const std::filesystem::path stagedDependencyPath = Files::BuildTemporaryPath(dependencyPath, ".cook-generation");

	const auto cleanupStagedFiles = [&]() noexcept
	{
		Files::CleanupTemporaryFile(stagedMapPath);
		Files::CleanupTemporaryFile(stagedLibraryPath);
		Files::CleanupTemporaryFile(stagedDependencyPath);
	};

	cleanupStagedFiles();

	try
	{
		ShaderCookOutput result = ShaderArtifactWriter::Write(plan, bReplaceCompleteCatalog, mapPath, libraryPath, stagedMapPath, stagedLibraryPath);
		result.mapPath = mapPath;
		result.libraryPath = libraryPath;
		const CookedShaderLibrary stagedLibrary = CookedShaderLibrary::Open(stagedLibraryPath);
		const GlobalShaderMap stagedMap = GlobalShaderMap::Open(stagedMapPath, stagedLibrary);
		ShaderDependencyManifest::Write(plan.dependencyManifest, stagedDependencyPath);
		std::vector<Files::FilePublication> files = {{stagedMapPath, mapPath}, {stagedLibraryPath, libraryPath}, {stagedDependencyPath, dependencyPath}};
		if (settings.bEnableDebugInfo || !settings.debugArtifactDirectory.empty())
		{
			ShaderDebugArtifactWriter debugWriter(debugDirectory);
			Files::FilePublication debugPublication = debugWriter.StageManifest(plan, compileResults, stagedMap, stagedLibrary, stagedMapPath, stagedLibraryPath);
			files.push_back(std::move(debugPublication));
			result.debugManifestPath = files.back().PublishedPath;
		}
		std::string error;
		if (!publisher.Publish(files, error))
		{
			throw Diagnostics::Error(error);
		}
		return result;
	}
	catch (...)
	{
		cleanupStagedFiles();
		throw;
	}
}
