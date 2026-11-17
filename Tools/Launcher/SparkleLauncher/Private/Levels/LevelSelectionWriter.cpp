#include "Levels/LevelSelectionWriter.h"

#include "ProjectLevelCatalog.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Strings/StringUtils.h"

#include <cstddef>
#include <optional>
#include <sstream>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

namespace SparkleLauncher
{
	static bool UpdateLevelSelection(std::vector<std::string>& lines, std::span<const std::string> levelIds, bool bSelected, std::string& outErrorMessage)
	{
		std::unordered_set<std::string_view> remainingLevelIds(levelIds.begin(), levelIds.end());
		const std::string valueLine = std::string("Selected = ") + (bSelected ? "true" : "false");
		bool bInLevelSection = false;
		std::string currentLevelId;
		std::optional<std::size_t> selectedLine;

		for (std::size_t index = 0; index < lines.size(); ++index)
		{
			const std::string trimmed = Strings::TrimCopy(lines[index]);
			if (trimmed.empty() || trimmed.front() == '#' || trimmed.front() == ';')
			{
				continue;
			}
			if (trimmed.front() == '[' && trimmed.back() == ']')
			{
				bInLevelSection = trimmed == "[Level]";
				currentLevelId.clear();
				selectedLine.reset();
				continue;
			}
			if (!bInLevelSection)
			{
				continue;
			}

			std::string_view key;
			std::string_view value;
			if (!Strings::TrySplitKeyValue(trimmed, '=', key, value))
			{
				continue;
			}
			if (key == "Id")
			{
				currentLevelId = Strings::UnquoteCopy(value);
			}
			else if (key == "Selected")
			{
				selectedLine = index;
			}

			if (selectedLine && remainingLevelIds.erase(currentLevelId) != 0)
			{
				lines[*selectedLine] = valueLine;
				selectedLine.reset();
			}
		}

		if (remainingLevelIds.empty())
		{
			return true;
		}

		outErrorMessage = "Project level catalog entry was not found: " + std::string(*remainingLevelIds.begin());
		return false;
	}

	bool WriteLevelSelection(const std::filesystem::path& projectRoot, std::span<const std::string> levelIds, bool bSelected, std::string& outErrorMessage)
	{
		outErrorMessage.clear();
		if (levelIds.empty())
		{
			return true;
		}

		const std::filesystem::path catalogPath = projectRoot / "Levels.catalog";
		std::string catalogText;
		if (!Files::TryReadAllText(catalogPath, catalogText, outErrorMessage))
		{
			return false;
		}

		try
		{
			(void) ParseProjectLevelCatalog(projectRoot, catalogText);
			std::istringstream input(catalogText);
			std::vector<std::string> lines;
			for (std::string line; std::getline(input, line);)
			{
				if (line.ends_with('\r'))
				{
					line.pop_back();
				}
				lines.push_back(std::move(line));
			}
			if (!UpdateLevelSelection(lines, levelIds, bSelected, outErrorMessage))
			{
				return false;
			}

			std::string updatedText;
			for (const std::string& line : lines)
			{
				updatedText += line;
				updatedText += '\n';
			}
			(void) ParseProjectLevelCatalog(projectRoot, updatedText);
			return Files::TryWriteAllTextAtomic(catalogPath, updatedText, outErrorMessage);
		}
		catch (const Diagnostics::Error& error)
		{
			outErrorMessage = std::string("Project level catalog update was rejected: ") + error.what();
			return false;
		}
	}
}
