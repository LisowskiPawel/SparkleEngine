#pragma once

#include <filesystem>
#include <memory>

class LevelAsset;
struct LevelDesc;

namespace LevelParser
{
	std::unique_ptr<LevelAsset> LoadFromFile(const std::filesystem::path& filePath);
	void SaveToFile(const LevelDesc& description, const std::filesystem::path& sourcePath);
}
