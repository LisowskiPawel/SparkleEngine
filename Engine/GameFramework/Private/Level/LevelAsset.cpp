#include "PCH.h"
#include "Level/LevelAsset.h"

#include "Level/Parsing/LevelParser.h"

#include <utility>

LevelAsset::LevelAsset(LevelDesc levelDesc, std::filesystem::path sourcePath) :
    m_levelDesc(std::move(levelDesc)),
    m_sourcePath(std::move(sourcePath))
{
}

LevelAsset::~LevelAsset() = default;

void LevelAsset::Save(LevelDesc description)
{
	LevelParser::SaveToFile(description, m_sourcePath);
	m_levelDesc = std::move(description);
}
