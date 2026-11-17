#pragma once

#include "ProjectLevelCatalog.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iosfwd>
#include <string>
#include <string_view>
#include <unordered_set>

class ProjectLevelCatalogReader final
{
public:
	static ProjectLevelCatalog Parse(const std::filesystem::path& projectRoot, std::istream& input);

private:
	enum class Section : std::uint8_t
	{
		None,
		Level,
		AssetPack
	};

	explicit ProjectLevelCatalogReader(const std::filesystem::path& projectRoot) noexcept;

	void ReadCatalog(std::istream& input);
	void ParseLine(std::string line);
	void BeginLevel();
	void BeginAssetPack();
	void ParseLevelField(std::string_view key, std::string_view value);
	void ParseAssetPackField(std::string_view key, std::string_view value);
	bool ParseBool(std::string_view value) const;
	std::uintmax_t ParseByteCount(std::string_view value) const;
	void ValidateCurrentSection() const;
	std::filesystem::path ResolveProjectPath(std::string_view value) const;

	const std::filesystem::path& m_projectRoot;
	ProjectLevelCatalog m_catalog;
	Section m_section = Section::None;
	ProjectLevelCatalogEntry* m_currentLevel = nullptr;
	ProjectAssetPack* m_currentPack = nullptr;
	std::unordered_set<std::string> m_sectionFields;
	std::size_t m_lineNumber = 0;
};
