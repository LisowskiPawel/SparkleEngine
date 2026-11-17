#pragma once

#include "GameFramework/Public/Level/LevelDesc.h"

#include <filesystem>
#include <string_view>

class LevelAsset final
{
public:
	LevelAsset(LevelDesc levelDesc, std::filesystem::path sourcePath = {});
	~LevelAsset();

	LevelAsset(const LevelAsset&) = delete;
	LevelAsset& operator=(const LevelAsset&) = delete;
	LevelAsset(LevelAsset&&) = delete;
	LevelAsset& operator=(LevelAsset&&) = delete;

	std::string_view GetName() const noexcept { return m_levelDesc.name; }

	const LevelDesc& GetDescription() const noexcept { return m_levelDesc; }

	void Save(LevelDesc description);

private:
	LevelDesc m_levelDesc;
	std::filesystem::path m_sourcePath;
};
