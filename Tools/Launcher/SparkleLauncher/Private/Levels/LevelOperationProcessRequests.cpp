#include "LevelOperationProcessRequests.h"
#include "Core/Public/Strings/StringUtils.h"

#include "Core/Public/Paths/ProductPaths.h"
#include "AssetPackSyncPlanner.h"

#include "Core/Public/Projects/ProjectLevelCatalog.h"
#include "LauncherStatePaths.h"
#include "Core/Public/Paths/PackagePaths.h"
#include "Core/Public/Diagnostics/Error.h"

#include <utility>

namespace SparkleLauncher
{
	static ProcessRequest MakeAssetPackSyncRequest(const LevelOperationPlan& plan, const ProjectAssetPack& pack)
	{
		const std::filesystem::path projectRoot = Filesystem::ResolveProductProjectRoot(plan.RepositoryRoot, plan.Request.ContentId);
		const LauncherStatePaths statePaths = ResolveLauncherStatePaths(plan.RepositoryRoot);
		const std::filesystem::path cacheRoot = statePaths.ContentArchivesRoot / plan.Request.ContentId;
		const std::filesystem::path rootRelativeToExtraction = pack.rootPath.lexically_relative(pack.extractionPath);

		std::filesystem::path syncScript = plan.RepositoryRoot / "Tools/Launcher/SparkleLauncher/Scripts/SyncAssetPack.cmake";
		if (!Filesystem::IsSourceWorkspace(plan.RepositoryRoot))
		{
			const auto verified = Filesystem::ResolvePackageFilePath(plan.RepositoryRoot, "Tools/Content/SyncAssetPack.cmake");
			if (!verified)
			{
				throw Diagnostics::Error("The declared content acquisition script is missing or corrupt.");
			}
			for (const std::string_view dependency : {"Tools/Content/SyncAssetPackFiles.cmake", "Tools/Content/SyncAssetPackArchive.cmake"})
			{
				if (!Filesystem::ResolvePackageFilePath(plan.RepositoryRoot, dependency))
				{
					throw Diagnostics::Error("A declared acquisition script dependency is missing or corrupt.");
				}
			}
			syncScript = *verified;
		}

		ProcessRequest process;
		process.WorkingDirectory = plan.RepositoryRoot;
		process.LogPath = ResolveLauncherOperationLogPath(plan.RepositoryRoot, plan.Operation.Id, "AssetPack-" + pack.id + ".txt");
		process.ExecutablePath = plan.CMakePath;

		process.Arguments = {
		    "-DSPARKLE_PACK_ID=" + pack.id,
		    "-DSPARKLE_PACK_URL=" + pack.sourceUrl,
		    "-DSPARKLE_PACK_ARCHIVE_NAME=" + pack.archiveName,
		    "-DSPARKLE_PACK_ARCHIVE_BYTES=" + std::to_string(pack.archiveBytes),
		    "-DSPARKLE_PACK_ARCHIVE_SHA256=" + pack.archiveSha256,
		    "-DSPARKLE_PACK_SOURCE_PAGE=" + pack.sourcePageUrl,
		    "-DSPARKLE_PACK_VERSION=" + pack.version,
		    "-DSPARKLE_PACK_LICENSE=" + pack.license,
		    "-DSPARKLE_PACK_PROJECT_ROOT=" + Strings::ToNarrow(projectRoot.generic_wstring()),
		    "-DSPARKLE_PACK_CACHE_ROOT=" + Strings::ToNarrow(cacheRoot.generic_wstring()),
		    "-DSPARKLE_PACK_EXTRACT_ROOT=" + Strings::ToNarrow(pack.extractionPath.generic_wstring()),
		    "-DSPARKLE_PACK_ROOT_RELATIVE=" + Strings::ToNarrow(rootRelativeToExtraction.generic_wstring()),
		    "-DSPARKLE_PACK_REQUIRED_RELATIVE=" + Strings::ToNarrow(pack.requiredRelativePath.generic_wstring()),
		    "-P",
		    Strings::ToNarrow(syncScript.generic_wstring()),
		};

		if (!pack.sourceFilesManifestPath.empty())
		{
			process.Arguments.insert(process.Arguments.end() - 2, "-DSPARKLE_PACK_FILES_MANIFEST=" + Strings::ToNarrow(pack.sourceFilesManifestPath.generic_wstring()));
		}
		return process;
	}

	std::vector<LevelOperationProcessStep> BuildLevelOperationProcessSteps(const LevelOperationPlan& plan)
	{
		std::vector<LevelOperationProcessStep> steps;
		const std::filesystem::path projectRoot = Filesystem::ResolveProductProjectRoot(plan.RepositoryRoot, plan.Request.ContentId);
		const ProjectLevelCatalog catalog = ProjectLevelCatalogFile::Load(projectRoot);
		for (const std::string& packId : BuildAssetPackSyncPlan(catalog, plan.Request.RequestedLevelIds))
		{
			const ProjectAssetPack& pack = catalog.assetPacks.at(packId);
			if (catalog.IsAssetPackPayloadPresent(pack))
			{
				continue;
			}

			LevelOperationProcessStep step;
			step.Id = "sync-asset-pack-" + pack.id;
			step.DisplayName = "Acquire " + pack.displayName;
			step.Request = MakeAssetPackSyncRequest(plan, pack);
			steps.push_back(std::move(step));
		}
		return steps;
	}
}
