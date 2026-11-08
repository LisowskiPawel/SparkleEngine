#pragma once

#include <filesystem>
#include <memory>

class LevelAsset;

namespace LevelParser
{
	std::unique_ptr<LevelAsset> LoadFromFile(const std::filesystem::path& filePath);
	void SaveToFile(const LevelAsset& level);
}
