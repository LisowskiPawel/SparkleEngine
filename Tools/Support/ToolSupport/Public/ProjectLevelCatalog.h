#pragma once

#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <vector>

struct ProjectLevelCatalogEntry final
{
	std::string id;
	std::string displayName;
	std::string description;
	std::filesystem::path sourcePath;
	std::filesystem::path thumbnailPath;
	std::string sourcePageUrl;
	std::string assetPackId;
	std::string family;
	std::string variantKind;
	bool bReleaseSupported = false;
	bool bSelected = false;
};

struct ProjectAssetPack final
{
	std::string id;
	std::string displayName;
	std::filesystem::path rootPath;
	std::filesystem::path extractionPath;
	std::filesystem::path requiredRelativePath;
	std::string parentPackId;
	std::string contentKind = "Scene";
	std::string sourceUrl;
	std::string sourcePageUrl;
	std::filesystem::path sourceFilesManifestPath;
	std::string archiveName;
	std::string archiveSha256;
	std::string version;
	std::string license;
	std::string gameBlocker;
	std::string downloadBlocker;
	std::uintmax_t archiveBytes = 0;
	bool bExternal = false;
	bool bDownloadSupported = false;
	bool bGameSupported = false;
};

struct ProjectLevelCatalog final
{
	std::vector<ProjectLevelCatalogEntry> levels;
	std::map<std::string, ProjectAssetPack, std::less<>> assetPacks;

	bool IsAssetPackPayloadPresent(const ProjectAssetPack& pack) const;
	bool IsAssetPackSourceReady(std::string_view packId) const;
	bool HasLevelSourceInputs(const ProjectLevelCatalogEntry& level) const;
};

class ProjectLevelCatalogFile final
{
public:
	static ProjectLevelCatalog Load(const std::filesystem::path& projectRoot);
	static bool SetLevelSelected(const std::filesystem::path& projectRoot, std::string_view levelId, bool bSelected, std::string& outErrorMessage);
	static bool SetLevelsSelected(const std::filesystem::path& projectRoot, const std::vector<std::string>& levelIds, bool bSelected, std::string& outErrorMessage);
};
