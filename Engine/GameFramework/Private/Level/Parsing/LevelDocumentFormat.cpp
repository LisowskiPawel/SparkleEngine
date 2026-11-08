#include "PCH.h"
#include "Level/Parsing/LevelDocumentFormat.h"

#include "Level/Parsing/CameraSectionParser.h"
#include "Level/Parsing/LevelParserCommon.h"
#include "Level/Parsing/LightingSectionParser.h"
#include "Level/Parsing/SkySectionParser.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"

#include <format>
#include <istream>
#include <ostream>
#include <utility>

static void ParseLevelSectionField(const LevelParsing::ParsedLevelLine& parsedLine, LevelDesc& levelDesc)
{
	if (parsedLine.key == "Name")
	{
		levelDesc.name = Strings::UnquoteCopy(parsedLine.value);
		if (levelDesc.name.empty())
		{
			throw Diagnostics::Error("Level name cannot be empty.");
		}
		return;
	}

	throw Diagnostics::Error("Unsupported level field: " + parsedLine.key);
}

static void ParseSceneAssetsSectionField(const LevelParsing::ParsedLevelLine& parsedLine, LevelDesc& levelDesc)
{
	if (parsedLine.key != "Asset")
	{
		throw Diagnostics::Error("Unsupported scene-assets field: " + parsedLine.key);
	}

	const std::string value = Strings::UnquoteCopy(parsedLine.value);
	const std::size_t separator = value.find('|');
	if (separator != std::string::npos && value.find('|', separator + 1u) != std::string::npos)
	{
		throw Diagnostics::Error("Scene asset reference contains multiple catalog separators.");
	}

	SceneAssetId assetId;
	assetId.value = Strings::TrimCopy(separator == std::string::npos ? std::string_view(value) : std::string_view(value).substr(0, separator));
	if (separator != std::string::npos)
	{
		assetId.catalogValue = Strings::TrimCopy(std::string_view(value).substr(separator + 1u));
	}
	if (assetId.value.empty() || (separator != std::string::npos && assetId.catalogValue.empty()))
	{
		throw Diagnostics::Error("Scene asset reference is empty.");
	}
	levelDesc.sceneAssetIds.push_back(std::move(assetId));
}

static void ParseField(LevelParsing::LevelFileSection currentSection, const LevelParsing::ParsedLevelLine& parsedLine, LevelDesc& levelDesc)
{
	switch (currentSection)
	{
		case LevelParsing::LevelFileSection::Level:
			ParseLevelSectionField(parsedLine, levelDesc);
			return;

		case LevelParsing::LevelFileSection::Camera:
			LevelParsing::ParseCameraSectionField(parsedLine, levelDesc);
			return;

		case LevelParsing::LevelFileSection::Sky:
			LevelParsing::ParseSkySectionField(parsedLine, levelDesc);
			return;

		case LevelParsing::LevelFileSection::Lighting:
			LevelParsing::ParseLightingSectionField(parsedLine, levelDesc);
			return;

		case LevelParsing::LevelFileSection::SceneAssets:
			ParseSceneAssetsSectionField(parsedLine, levelDesc);
			return;

		case LevelParsing::LevelFileSection::None:
		default:
			throw Diagnostics::Error("Level field appears outside a supported section.");
	}
}

static void WriteLevelSection(std::ostream& output, const LevelDesc& levelDesc)
{
	output << "[Level]\n";
	output << "Name = " << levelDesc.name << "\n\n";
}

static void WriteSceneAssetsSection(std::ostream& output, const LevelDesc& levelDesc)
{
	output << "[SceneAssets]\n";
	for (const SceneAssetId& sceneAssetId : levelDesc.sceneAssetIds)
	{
		output << "Asset = " << sceneAssetId.value;
		if (!sceneAssetId.catalogValue.empty() && sceneAssetId.catalogValue != sceneAssetId.value)
		{
			output << '|' << sceneAssetId.catalogValue;
		}
		output << "\n";
	}
}

static void ParseDescriptionLine(std::string line, LevelParsing::LevelFileSection& section, LevelDesc& description)
{
	line = Strings::TrimCopy(line);
	if (line.empty() || line[0] == '#' || line[0] == ';')
	{
		return;
	}
	if (line.front() == '[' && line.back() == ']')
	{
		section = LevelParsing::ParseSection(line);
		return;
	}
	ParseField(section, LevelParsing::ParseField(line), description);
}

static LevelDesc ReadDescription(std::istream& input, const std::filesystem::path& sourcePath)
{
	LevelParsing::LevelFileSection section = LevelParsing::LevelFileSection::None;
	LevelDesc description;
	std::size_t lineNumber = 0;
	for (std::string line; std::getline(input, line);)
	{
		++lineNumber;
		try
		{
			ParseDescriptionLine(std::move(line), section, description);
		}
		catch (const Diagnostics::Error& error)
		{
			throw Diagnostics::Error(std::format("{} ({}:{})", error.what(), sourcePath.string(), lineNumber));
		}
	}
	if (input.bad())
	{
		throw Diagnostics::Error(std::format("Failed while reading level file '{}'.", sourcePath.string()));
	}
	return description;
}

LevelDesc LevelParsing::ReadLevelDescription(std::istream& input, const std::filesystem::path& sourcePath)
{
	LevelDesc description = ReadDescription(input, sourcePath);
	if (description.name.empty())
	{
		throw Diagnostics::Error(std::format("Level file '{}' has no level name.", sourcePath.string()));
	}
	ValidateCameraSection(description);
	ValidateLightingSection(description);
	return description;
}

void LevelParsing::WriteLevelDescription(std::ostream& output, const LevelDesc& description)
{
	WriteLevelSection(output, description);
	WriteCameraSection(output, description);
	WriteSkySection(output, description);
	WriteLightingSection(output, description);
	WriteSceneAssetsSection(output, description);
}
