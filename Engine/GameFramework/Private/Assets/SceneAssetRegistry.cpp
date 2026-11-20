#include "PCH.h"
#include "Core/Public/Paths/CookedAssetPaths.h"

#include "Assets/SceneAssetRegistry.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"

#include <fstream>
#include <format>
#include <utility>

namespace Assets
{
	constexpr std::string_view kSceneAssetRegistryHeader = "[SceneAssetRegistry]";
	constexpr std::string_view kSceneAssetEntriesHeader = "[Entries]";

	using SceneAssetRegistryEntries = std::map<std::string, std::filesystem::path, std::less<>>;

	static SceneAssetRegistryEntries ReadRegistryEntries(std::istream& input, const std::filesystem::path& registryPath);
	static std::pair<std::string, std::filesystem::path> ParseRegistryEntry(std::string_view entryValue, const std::filesystem::path& registryPath, std::size_t lineNumber);

	static SceneAssetRegistryEntries ReadRegistry(const std::filesystem::path& registryPath)
	{
		std::error_code errorCode;
		const bool bExists = std::filesystem::exists(registryPath, errorCode);
		if (errorCode)
		{
			throw Diagnostics::Error(std::format("Could not inspect scene asset registry '{}': {}.", registryPath.string(), errorCode.message()));
		}
		if (!bExists)
		{
			return {};
		}

		std::ifstream input(registryPath);
		if (!input.is_open())
		{
			throw Diagnostics::Error("Could not open scene asset registry '" + registryPath.string() + "'.");
		}

		return ReadRegistryEntries(input, registryPath);
	}

	static SceneAssetRegistryEntries ReadRegistryEntries(std::istream& input, const std::filesystem::path& registryPath)
	{
		SceneAssetRegistryEntries entries;
		bool bFoundRegistryHeader = false;
		bool bFoundEntriesHeader = false;
		bool bInEntriesSection = false;
		std::size_t lineNumber = 0;
		for (std::string line; std::getline(input, line);)
		{
			++lineNumber;
			const std::string trimmedLine = Strings::TrimCopy(line);
			if (trimmedLine.empty() || trimmedLine[0] == '#' || trimmedLine[0] == ';')
			{
				continue;
			}

			if (trimmedLine == kSceneAssetRegistryHeader)
			{
				if (bFoundRegistryHeader)
				{
					throw Diagnostics::Error(std::format("Scene asset registry '{}' repeats its header at line {}.", registryPath.string(), lineNumber));
				}
				bFoundRegistryHeader = true;
				bInEntriesSection = false;
				continue;
			}

			if (trimmedLine == kSceneAssetEntriesHeader)
			{
				if (!bFoundRegistryHeader || bFoundEntriesHeader)
				{
					throw Diagnostics::Error(std::format("Scene asset registry '{}' has an invalid entries section at line {}.", registryPath.string(), lineNumber));
				}
				bFoundEntriesHeader = true;
				bInEntriesSection = true;
				continue;
			}

			std::string_view key;
			std::string_view value;
			const bool bEntryDecoded = bInEntriesSection && Strings::TrySplitKeyValue(trimmedLine, '=', key, value);
			const bool bEntryKeyMatches = bEntryDecoded && Strings::EqualsIgnoreCase(key, "Entry");

			if (!bEntryKeyMatches)
			{
				throw Diagnostics::Error(std::format("Scene asset registry '{}' has an invalid field at line {}.", registryPath.string(), lineNumber));
			}

			auto [sceneAssetId, manifestRelativePath] = ParseRegistryEntry(value, registryPath, lineNumber);
			if (!entries.emplace(std::move(sceneAssetId), std::move(manifestRelativePath)).second)
			{
				throw Diagnostics::Error(std::format("Scene asset registry '{}' repeats an asset identity at line {}.", registryPath.string(), lineNumber));
			}
		}

		if (input.bad() || !input.eof())
		{
			throw Diagnostics::Error("Could not read scene asset registry '" + registryPath.string() + "'.");
		}
		if (!bFoundRegistryHeader || !bFoundEntriesHeader)
		{
			throw Diagnostics::Error("Scene asset registry '" + registryPath.string() + "' has an incomplete structure.");
		}
		return entries;
	}

	static std::pair<std::string, std::filesystem::path> ParseRegistryEntry(std::string_view entryValue, const std::filesystem::path& registryPath, std::size_t lineNumber)
	{
		const std::size_t separatorIndex = entryValue.find('|');
		if (separatorIndex == std::string_view::npos || entryValue.find('|', separatorIndex + 1) != std::string_view::npos)
		{
			throw Diagnostics::Error(std::format("Scene asset registry '{}' has an invalid entry at line {}.", registryPath.string(), lineNumber));
		}

		const std::string_view identityValue = entryValue.substr(0, separatorIndex);
		const std::string_view manifestPathValue = entryValue.substr(separatorIndex + 1);
		std::string sceneAssetId = Strings::TrimCopy(identityValue);
		std::filesystem::path manifestRelativePath = Strings::TrimCopy(manifestPathValue);
		const bool bInvalidPath = manifestRelativePath.empty() || manifestRelativePath.has_root_path();
		if (sceneAssetId.empty() || bInvalidPath)
		{
			throw Diagnostics::Error(std::format("Scene asset registry '{}' has an invalid entry at line {}.", registryPath.string(), lineNumber));
		}
		for (const std::filesystem::path& component : manifestRelativePath)
		{
			if (component == "..")
			{
				throw Diagnostics::Error(std::format("Scene asset registry '{}' escapes its asset root at line {}.", registryPath.string(), lineNumber));
			}
		}
		return {std::move(sceneAssetId), std::move(manifestRelativePath)};
	}

	void SceneAssetRegistry::Load()
	{
		const std::filesystem::path& registryPath = Filesystem::GetSceneAssetRegistryPath();
		m_entries = ReadRegistry(registryPath);
	}

	void SceneAssetRegistry::Save(const std::filesystem::path& outputPath) const
	{
		if (outputPath.empty())
		{
			throw Diagnostics::Error("Scene asset registry output path is empty.");
		}

		std::error_code errorCode;
		const std::filesystem::path outputDirectory = outputPath.parent_path();
		if (!outputDirectory.empty())
		{
			std::filesystem::create_directories(outputDirectory, errorCode);
		}
		if (errorCode)
		{
			throw Diagnostics::Error(std::format("Could not create scene asset registry directory '{}': {}.", outputDirectory.string(), errorCode.message()));
		}

		std::ofstream output(outputPath, std::ios::trunc);
		if (!output.is_open())
		{
			throw Diagnostics::Error("Could not open scene asset registry for writing '" + outputPath.string() + "'.");
		}

		output << kSceneAssetRegistryHeader << "\n\n";
		output << kSceneAssetEntriesHeader << "\n";
		for (const auto& [sceneAssetId, manifestRelativePath] : m_entries)
		{
			output << "Entry = " << sceneAssetId << "|" << manifestRelativePath.generic_string() << "\n";
		}

		if (!output.good())
		{
			throw Diagnostics::Error("Could not write scene asset registry '" + outputPath.string() + "'.");
		}
	}

	void SceneAssetRegistry::Upsert(std::string sceneAssetId, std::filesystem::path sceneManifestRelativePath)
	{
		m_entries[std::move(sceneAssetId)] = std::move(sceneManifestRelativePath);
	}

	std::map<std::string, std::filesystem::path, std::less<>> SceneAssetRegistry::ReleaseEntries() noexcept
	{
		return std::move(m_entries);
	}
}
