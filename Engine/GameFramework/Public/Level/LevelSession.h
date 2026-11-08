#pragma once

#include "GameFramework/Public/GameFrameworkAPI.h"
#include "Level/LevelChangeEvents.h"
#include "Level/LevelLoadOperation.h"

#include <memory>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

class LevelAsset;
class LevelRegistry;
class GameWorld;

namespace Assets
{
	class SceneLoadExecutor;
	struct SceneLoadCompletion;
	struct SceneLoadPackage;
}
class TaskExecutor;
class TaskScope;

class SPARKLE_ENGINE_API LevelSession final
{
public:
	LevelSession(GameWorld& world, TaskExecutor& taskExecutor, TaskScope& applicationScope);
	~LevelSession() noexcept;

	LevelSession(const LevelSession&) = delete;
	LevelSession& operator=(const LevelSession&) = delete;
	LevelSession(LevelSession&&) = delete;
	LevelSession& operator=(LevelSession&&) = delete;

	bool HasActiveLevel() const noexcept { return m_activeLevel != nullptr; }

	bool IsLevelChangeInProgress() const noexcept { return m_bLevelChangeInProgress; }

	std::vector<std::string> GetRegisteredLevelNames() const;

	LevelChangeEvents& GetLevelChangeEvents() noexcept { return m_levelChangeEvents; }

	const LevelChangeEvents& GetLevelChangeEvents() const noexcept { return m_levelChangeEvents; }

	LevelLoadOperationProgress GetLoadProgress() const noexcept;

	std::string_view GetLastLoadDiagnostic() const noexcept { return m_lastLoadDiagnostic; }

	void RequestLevelChange(std::string_view requestedLevelName) noexcept;
	void ProcessPendingLevelChange() noexcept;
	void RequestLevelReload() noexcept;

	LevelAsset* GetActiveLevel() noexcept { return m_activeLevel; }

	const LevelAsset* GetActiveLevel() const noexcept { return m_activeLevel; }

	bool SaveActiveLevel() noexcept;

private:
	void InitializeStartupLevel() noexcept;
	void StartLevelChange(LevelAsset& requestedLevel) noexcept;
	void CompleteLevelChange() noexcept;
	LevelAsset& TakeLoadingLevel(std::uint64_t requestId) noexcept;
	void RejectLevelLoad(LevelAsset& level, std::string diagnostic) noexcept;
	void ActivateLoadedLevel(LevelAsset& level, Assets::SceneLoadPackage package) noexcept;
	void NotifyLevelUnloading(const std::string& previousName, const std::string& requestedName);
	void NotifyLevelChanged(const std::string& previousName, const std::string& activeName);

	GameWorld* m_gameWorld = nullptr;
	std::unique_ptr<LevelRegistry> m_levelRegistry;
	std::unique_ptr<Assets::SceneLoadExecutor> m_loadExecutor;
	LevelChangeEvents m_levelChangeEvents;
	LevelAsset* m_activeLevel = nullptr;
	LevelAsset* m_pendingLevelChange = nullptr;
	LevelAsset* m_loadingLevel = nullptr;
	std::uint64_t m_nextRequestId = 1;
	std::uint64_t m_latestRequestId = 0;
	std::uint64_t m_documentGeneration = 1;
	std::string m_lastLoadDiagnostic;
	bool m_bLevelChangeInProgress = false;
};
