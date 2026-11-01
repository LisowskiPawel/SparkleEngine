#include "PCH.h"
#include "Level/LevelRegistry.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Level/Level.h"
#include "Level/Parsing/LevelParser.h"

#include <format>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_levelRegistryLogger, "GameFramework.LevelRegistry");

LevelRegistry::LevelRegistry(const std::filesystem::path& projectRoot)
{
	DiscoverLevels(projectRoot);
}

LevelRegistry::~LevelRegistry() noexcept = default;

void LevelRegistry::DiscoverLevels(const std::filesystem::path& projectRoot)
{
	std::error_code error;
	std::filesystem::directory_iterator iterator(projectRoot / "Levels", error);
	const std::filesystem::directory_iterator end;
	while (!error && iterator != end)
	{
		const auto& entry = *iterator;
		if (entry.is_regular_file(error) && entry.path().extension() == ".level")
		{
			LoadLevelFile(entry.path());
		}
		if (!error)
		{
			iterator.increment(error);
		}
	}
	if (error)
	{
		SPDLOG_LOGGER_WARN(g_levelRegistryLogger, "LevelRegistry: could not discover level documents in '{}': {}.", projectRoot.string(), error.message());
	}
	EnsureDefaultLevel();
}

void LevelRegistry::LoadLevelFile(const std::filesystem::path& path)
{
	try
	{
		auto loadedLevel = LevelParser::LoadFromFile(path);
		const std::string loadedLevelName(loadedLevel->GetName());
		if (loadedLevelName != path.stem().string())
		{
			throw Diagnostics::Error(std::format("Level filename identity '{}' does not match authored level name '{}'.", path.stem().string(), loadedLevelName));
		}
		Register(std::move(loadedLevel));
	}
	catch (const Diagnostics::Error& error)
	{
		const std::string failure = std::format("LevelRegistry: Failed to load level document '{}': {}", path.string(), error.what());
		SPDLOG_LOGGER_WARN(g_levelRegistryLogger, "{}", failure);
	}
}

void LevelRegistry::EnsureDefaultLevel()
{
	if (FindLevel("Empty") == nullptr)
	{
		LevelDesc fallbackLevel;
		fallbackLevel.name = "Empty";
		Register(std::make_unique<LevelAsset>(std::move(fallbackLevel)));
	}
	m_defaultLevelName = "Empty";
}

void LevelRegistry::Register(std::unique_ptr<LevelAsset> level)
{
	if (!level)
	{
		Diagnostics::Fatal(g_levelRegistryLogger, __FILE__, __LINE__, "LevelRegistry: Attempted to register a null level.");
	}

	auto name = level->GetName();
	std::string nameKey(name);

	if (m_levels.contains(nameKey))
	{
		Diagnostics::Fatal(g_levelRegistryLogger, __FILE__, __LINE__, std::format("LevelRegistry: Duplicate level name '{}'.", nameKey));
	}

	m_levels.emplace(std::move(nameKey), std::move(level));
}

LevelAsset* LevelRegistry::FindLevel(std::string_view name) const
{
	if (name.empty())
	{
		return nullptr;
	}

	auto it = m_levels.find(std::string(name));
	return it != m_levels.end() ? it->second.get() : nullptr;
}

std::vector<std::string> LevelRegistry::GetLevelNames() const
{
	std::vector<std::string> levelNames;
	levelNames.reserve(m_levels.size());
	for (const auto& levelEntry : m_levels)
	{
		levelNames.push_back(levelEntry.first);
	}
	std::sort(levelNames.begin(), levelNames.end());
	return levelNames;
}

void LevelRegistry::SaveLevel(const LevelAsset& level) const
{
	if (FindLevel(level.GetName()) == nullptr)
	{
		throw Diagnostics::Error("Level not found.");
	}

	LevelParser::SaveToFile(level);
}

std::string_view LevelRegistry::GetDefaultLevelName() const noexcept
{
	return m_defaultLevelName;
}
