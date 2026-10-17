#include "PCH.h"

#include "Cooking/ShaderArtifactPublication.h"

#include "Cooking/Dependencies/ShaderDependencyManifest.h"
#include "Cooking/ShaderArtifactWriter.h"
#include "Cooking/ShaderRecookSignal.h"
#include "Cooking/ShaderDebugArtifactWriter.h"
#include "Cooking/ShaderCookSettings.h"
#include "Cooking/ShaderCookCancellation.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Paths/DirectoryPaths.h"
#include "RHI/Public/Shaders/GlobalShaderMap.h"

#include <utility>

ShaderCookOutput ShaderArtifactPublication::Publish(
    const ShaderCookPipelinePlan& plan,
    std::span<const ShaderCompileResult> compileResults,
    const ShaderCookSettings& settings,
    const std::filesystem::path& outputDirectory,
    bool replaceCompleteCatalog)
{
	const std::filesystem::path& mapPath = Filesystem::GetGlobalShaderMapPath();
	const std::filesystem::path& libraryPath = Filesystem::GetCookedShaderLibraryPath();
	const std::filesystem::path stagedMapPath = Files::BuildTemporaryPath(mapPath, ".cook-generation");
	const std::filesystem::path stagedLibraryPath = Files::BuildTemporaryPath(libraryPath, ".cook-generation");
	const std::filesystem::path dependencyPath = ShaderDependencyManifest::GetPath(outputDirectory);
	const std::filesystem::path stagedDependencyPath = Files::BuildTemporaryPath(dependencyPath, ".cook-generation");
	const std::filesystem::path signalPath = Paths::ShaderRecookSignal(outputDirectory);
	const std::filesystem::path stagedSignalPath = Files::BuildTemporaryPath(signalPath, ".cook-generation");

	const auto cleanupStagedFiles = [&]() noexcept
	{
		Files::CleanupTemporaryFile(stagedMapPath);
		Files::CleanupTemporaryFile(stagedLibraryPath);
		Files::CleanupTemporaryFile(stagedDependencyPath);
		Files::CleanupTemporaryFile(stagedSignalPath);
	};

	cleanupStagedFiles();

	try
	{
		ShaderCookOutput result = ShaderArtifactWriter::Write(plan, replaceCompleteCatalog, mapPath, libraryPath, stagedMapPath, stagedLibraryPath);
		result.mapPath = mapPath;
		result.libraryPath = libraryPath;
		const CookedShaderLibrary stagedLibrary = CookedShaderLibrary::Open(stagedLibraryPath);
		const GlobalShaderMap stagedMap = GlobalShaderMap::Open(stagedMapPath, stagedLibrary);
		ShaderDependencyManifest::Write(plan.dependencyManifest, stagedDependencyPath);
		ShaderRecookSignal::Write(stagedMapPath, stagedLibraryPath, stagedSignalPath);
		std::vector<Files::FilePublication> files = {{stagedMapPath, mapPath}, {stagedLibraryPath, libraryPath}, {stagedDependencyPath, dependencyPath}, {stagedSignalPath, signalPath}};
		if (settings.enableDebugInfo || !settings.debugArtifactDirectory.empty())
		{
			ShaderDebugArtifactWriter debugWriter(settings.debugArtifactDirectory.empty() ? Filesystem::GetShaderSymbolsOutputPath() : settings.debugArtifactDirectory);
			Files::FilePublication debugPublication = debugWriter.StageManifest(plan, compileResults, stagedMap, stagedLibrary, stagedMapPath, stagedLibraryPath);
			files.push_back(std::move(debugPublication));
			result.debugManifestPath = files.back().PublishedPath;
		}
		ShaderCookCancellation::ThrowIfRequested(settings.cancellationSignalPath);
		std::string error;
		if (!Files::TryPublishFileSet(files, error))
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
