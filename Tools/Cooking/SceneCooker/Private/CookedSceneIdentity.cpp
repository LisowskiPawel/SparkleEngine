#include "PCH.h"

#include "SceneCooker.h"
#include "CookedSceneBuild.h"
#include "SourceAssetPaths.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Paths/ProductFilePaths.h"

#include <optional>

static std::filesystem::path ResolveSourceScenePath(const std::filesystem::path& sourceScenePath)
{
	if (const auto resolvedPath = SparkleTools::ResolveSourceAssetPath(sourceScenePath, SparkleTools::SourceAssetType::Mesh))
	{
		return *resolvedPath;
	}

	throw Diagnostics::Error("Unable to resolve source scene path '" + sourceScenePath.string() + "'.");
}

static std::string BuildSceneAssetId(const std::filesystem::path& resolvedSourceScenePath)
{
	const std::filesystem::path projectMeshRoot = SparkleTools::GetSourceAssetRoot(SparkleTools::SourceAssetType::Mesh, SparkleTools::SourceAssetRoot::Project);
	const std::filesystem::path engineMeshRoot = SparkleTools::GetSourceAssetRoot(SparkleTools::SourceAssetType::Mesh, SparkleTools::SourceAssetRoot::Engine);

	std::optional<std::filesystem::path> relativePath = Paths::TryMakeRelativeUnderRoot(resolvedSourceScenePath, projectMeshRoot);
	if (!relativePath)
	{
		relativePath = Paths::TryMakeRelativeUnderRoot(resolvedSourceScenePath, engineMeshRoot);
	}

	if (!relativePath)
	{
		throw Diagnostics::Error("Source scene path must be under a Sparkle mesh asset root to derive a stable scene asset id: '" + resolvedSourceScenePath.string() + "'.");
	}

	std::filesystem::path sceneAssetPath(relativePath->generic_string());
	sceneAssetPath.replace_extension();
	return sceneAssetPath.generic_string();
}

CookedSceneIdentity SceneCooker::ResolveSceneIdentity(const std::filesystem::path& sourceScenePath)
{
	const std::filesystem::path resolvedSourceScenePath = ResolveSourceScenePath(sourceScenePath);
	CookedSceneIdentity identity;
	identity.assetId = BuildSceneAssetId(resolvedSourceScenePath);
	identity.manifestPath = Paths::CookedSceneManifest(identity.assetId);
	return identity;
}
