#include "PCH.h"
#include "Level/LevelSession.h"
#include "Core/Public/Paths/FileSystemPaths.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Environment/EnvironmentVariables.h"
#include "Level/Level.h"
#include "Level/LevelRegistry.h"
#include "Level/LevelWorldProjection.h"
#include "Level/Loading/SceneLoadExecutor.h"
#include "World/GameWorld.h"

#include <utility>
#include <format>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_levelSessionLogger, "GameFramework.LevelSession");

LevelSession::LevelSession(GameWorld& world, TaskExecutor& taskExecutor, TaskScope& applicationScope) :
    m_gameWorld(&world),
    m_levelRegistry(std::make_unique<LevelRegistry>(Filesystem::GetProjectPath())),
    m_loadExecutor(std::make_unique<Assets::SceneLoadExecutor>(taskExecutor, applicationScope))
{
	InitializeStartupLevel();
}

LevelSession::~LevelSession() noexcept = default;

void LevelSession::InitializeStartupLevel() noexcept
{
	std::string requestedName;
	Environment::TryGetVariable("SPARKLE_STARTUP_LEVEL", requestedName);
	const std::string_view startupName = requestedName.empty() ? m_levelRegistry->GetDefaultLevelName() : std::string_view(requestedName);
	LevelAsset* startupLevel = m_levelRegistry->FindLevel(startupName);
	if (!startupLevel && !requestedName.empty())
	{
		m_lastLoadDiagnostic = std::format("Requested level '{}' has no valid level document. No level was activated.", requestedName);
		SPDLOG_LOGGER_ERROR(g_levelSessionLogger, "Requested startup level '{}' is not registered; no level will be activated.", requestedName);
		return;
	}
	if (!startupLevel)
	{
		return;
	}
	m_pendingLevelChange = startupLevel;
}

std::vector<std::string> LevelSession::GetRegisteredLevelNames() const
{
	return m_levelRegistry->GetLevelNames();
}

void LevelSession::RequestLevelChange(std::string_view requestedLevelName) noexcept
{
	if (requestedLevelName.empty())
	{
		return;
	}
	LevelAsset* requestedLevel = m_levelRegistry->FindLevel(requestedLevelName);
	if (!requestedLevel)
	{
		m_lastLoadDiagnostic = std::format("Requested level '{}' has no valid level document. The active level is unchanged.", requestedLevelName);
		m_levelChangeEvents.OnLevelLoadFailed.Broadcast(requestedLevelName);
		SPDLOG_LOGGER_WARN(g_levelSessionLogger, "Requested level '{}' is not registered.", std::string(requestedLevelName));
		return;
	}
	if (!m_bLevelChangeInProgress && m_activeLevel == requestedLevel)
	{
		return;
	}
	if (m_pendingLevelChange == requestedLevel || (m_bLevelChangeInProgress && m_loadingLevel == requestedLevel))
	{
		return;
	}
	m_pendingLevelChange = requestedLevel;
}

void LevelSession::RequestLevelReload() noexcept
{
	if (m_activeLevel && !m_bLevelChangeInProgress)
	{
		m_pendingLevelChange = m_activeLevel;
	}
}

void LevelSession::ProcessPendingLevelChange() noexcept
{
	if (m_pendingLevelChange && m_bLevelChangeInProgress)
	{
		m_loadExecutor->Cancel();
		CompleteLevelChange();
		if (m_bLevelChangeInProgress)
		{
			return;
		}
	}
	if (m_pendingLevelChange)
	{
		LevelAsset* requestedLevel = m_pendingLevelChange;
		m_pendingLevelChange = nullptr;
		StartLevelChange(*requestedLevel);
	}
	CompleteLevelChange();
}

void LevelSession::StartLevelChange(LevelAsset& requestedLevel) noexcept
{
	const std::string previousLevelName = m_activeLevel ? std::string(m_activeLevel->GetName()) : std::string{};
	const std::string requestedLevelName(requestedLevel.GetName());
	const std::uint64_t requestId = m_nextRequestId++;

	m_loadExecutor->Cancel();
	m_latestRequestId = requestId;
	m_loadingLevel = &requestedLevel;
	m_bLevelChangeInProgress = true;
	m_lastLoadDiagnostic.clear();

	LevelChangeStartedEventArgs startedArgs;
	startedArgs.previousLevelName = previousLevelName;
	startedArgs.requestedLevelName = requestedLevelName;
	m_levelChangeEvents.OnLevelChangeStarted.Broadcast(startedArgs);
	m_levelChangeEvents.OnLevelWillLoad.Broadcast(requestedLevelName);

	try
	{
		m_loadExecutor->Start(requestId, m_gameWorld->GetGeneration(), m_documentGeneration, requestedLevel.BuildDescription());
	}
	catch (const Diagnostics::Error& error)
	{
		m_lastLoadDiagnostic = error.what();
		m_loadingLevel = nullptr;
		m_bLevelChangeInProgress = false;
		SPDLOG_LOGGER_ERROR(g_levelSessionLogger, "Failed to start loading level '{}': {}", requestedLevelName, m_lastLoadDiagnostic);
		m_levelChangeEvents.OnLevelLoadFailed.Broadcast(requestedLevelName);
	}
}

LevelAsset& LevelSession::TakeLoadingLevel(std::uint64_t requestId) noexcept
{
	if (requestId != m_latestRequestId)
	{
		Diagnostics::Fatal(g_levelSessionLogger, __FILE__, __LINE__, "Scene-load completion does not match the active request.");
	}
	if (!m_loadingLevel)
	{
		Diagnostics::Fatal(g_levelSessionLogger, __FILE__, __LINE__, "Scene-load completion has no active level.");
	}
	LevelAsset& level = *m_loadingLevel;
	m_loadingLevel = nullptr;
	return level;
}

void LevelSession::RejectLevelLoad(LevelAsset& level, std::string diagnostic) noexcept
{
	m_lastLoadDiagnostic = std::move(diagnostic);
	m_bLevelChangeInProgress = false;
	SPDLOG_LOGGER_ERROR(g_levelSessionLogger, "Failed to load level '{}': {}", std::string(level.GetName()), m_lastLoadDiagnostic);
	m_levelChangeEvents.OnLevelLoadFailed.Broadcast(level.GetName());
}

void LevelSession::NotifyLevelUnloading(const std::string& previousName, const std::string& requestedName)
{
	LevelWillUnloadEventArgs args;
	args.previousLevelName = previousName;
	args.requestedLevelName = requestedName;
	m_levelChangeEvents.OnLevelWillUnload.Broadcast(args);
	if (!previousName.empty())
	{
		m_levelChangeEvents.OnLevelUnloaded.Broadcast(previousName);
	}
}

void LevelSession::NotifyLevelChanged(const std::string& previousName, const std::string& activeName)
{
	LevelChangedEventArgs args;
	args.previousLevelName = previousName;
	args.activeLevelName = activeName;
	m_levelChangeEvents.OnLevelChanged.Broadcast(args);
}

void LevelSession::ActivateLoadedLevel(LevelAsset& level, Assets::SceneLoadPackage package) noexcept
{
	const std::string previousName = m_activeLevel ? std::string(m_activeLevel->GetName()) : std::string{};
	const std::string loadedName(level.GetName());
	m_gameWorld->CommitSceneLoadPackage(std::move(package));
	NotifyLevelUnloading(previousName, loadedName);
	m_gameWorld->FinalizeSceneLoadCommit();
	m_activeLevel = &level;
	SPDLOG_LOGGER_INFO(g_levelSessionLogger, "Activated level '{}'.", loadedName);
	g_levelSessionLogger->flush();
	NotifyLevelChanged(previousName, loadedName);
	m_bLevelChangeInProgress = false;
	m_lastLoadDiagnostic.clear();
}

void LevelSession::CompleteLevelChange() noexcept
{
	std::optional<Assets::SceneLoadCompletion> completion = m_loadExecutor->ConsumeSettled();
	if (!completion)
	{
		return;
	}

	LevelAsset& level = TakeLoadingLevel(completion->RequestId);
	if (completion->Stage == LevelLoadOperationStage::Cancelled)
	{
		m_lastLoadDiagnostic = std::move(completion->Diagnostic);
		m_bLevelChangeInProgress = false;
		return;
	}
	if (completion->Stage != LevelLoadOperationStage::Ready)
	{
		RejectLevelLoad(level, std::move(completion->Diagnostic));
		return;
	}
	if (!completion->Package)
	{
		Diagnostics::Fatal(g_levelSessionLogger, __FILE__, __LINE__, "Ready scene-load completion has no package.");
	}

	const Assets::SceneLoadPackage& package = *completion->Package;
	const bool bStaleOwners = package.WorldGeneration != m_gameWorld->GetGeneration() || package.DocumentGeneration != m_documentGeneration;
	if (bStaleOwners || package.CatalogGeneration != m_loadExecutor->GetCatalogGeneration())
	{
		RejectLevelLoad(level, "Scene load package was rejected because an owner generation changed.");
		return;
	}
	ActivateLoadedLevel(level, std::move(*completion->Package));
}

LevelLoadOperationProgress LevelSession::GetLoadProgress() const noexcept
{
	return m_loadExecutor->GetProgress();
}

bool LevelSession::SaveActiveLevel() noexcept
{
	if (!m_activeLevel || m_bLevelChangeInProgress)
	{
		return false;
	}
	m_activeLevel->SetLevelDesc(CaptureLevelWorldState(m_activeLevel->BuildDescription(), m_gameWorld->AcquireReadView()));
	try
	{
		m_levelRegistry->SaveLevel(*m_activeLevel);
		m_lastLoadDiagnostic.clear();
		return true;
	}
	catch (const Diagnostics::Error& error)
	{
		m_lastLoadDiagnostic = error.what();
		return false;
	}
}
