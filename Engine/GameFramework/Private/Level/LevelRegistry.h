#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class LevelAsset;

class LevelRegistry final
{
public:
	explicit LevelRegistry(const std::filesystem::path& projectRoot);
	~LevelRegistry() noexcept;

	LevelRegistry(const LevelRegistry&) = delete;
	LevelRegistry& operator=(const LevelRegistry&) = delete;
	LevelRegistry(LevelRegistry&&) = delete;
	LevelRegistry& operator=(LevelRegistry&&) = delete;

	LevelAsset* FindLevel(std::string_view name) const;

	std::vector<std::string> GetLevelNames() const;

	void SaveLevel(const LevelAsset& level) const;

	std::string_view GetDefaultLevelName() const noexcept;

private:
	void DiscoverLevels(const std::filesystem::path& projectRoot);
	void LoadLevelFile(const std::filesystem::path& path);
	void EnsureDefaultLevel();
	void Register(std::unique_ptr<LevelAsset> level);

	std::unordered_map<std::string, std::unique_ptr<LevelAsset>> m_levels;
	std::string m_defaultLevelName;
};
