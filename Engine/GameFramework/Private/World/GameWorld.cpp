#include "PCH.h"
#include "World/GameWorld.h"

#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "GameFramework/Public/Rendering/RenderFrameSubmission.h"
#include "GameFramework/Public/Scene/Camera/CameraNavigation.h"
#include "GameFramework/Public/World/WorldEditCommand.h"
#include "GameFramework/Public/World/WorldMaterialVariantView.h"
#include "World/Loading/SceneLoadWorldStaging.h"
#include "World/GameWorldState.h"
#include "World/Extraction/RenderFrameSubmissionExtractor.h"
#include "World/Editing/WorldEditCommandQueue.h"
#include "Level/Loading/SceneLoadPackage.h"
#include "World/Resources/GameWorldResourceStores.h"
#include "World/Systems/GameWorldSystems.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_gameWorldLogger, "GameFramework.GameWorld");

GameWorld::GameWorld(TaskExecutor& taskExecutor) :
    m_state(std::make_unique<ECS::GameWorldState>()),
    m_resources(std::make_unique<GameWorldResourceStores>()),
    m_taskExecutor(taskExecutor),
    m_editCommands(std::make_unique<WorldEditCommandQueue>()),
    m_renderFrameSubmissionExtractor(std::make_unique<ECS::RenderFrameSubmissionExtractor>())
{
}

GameWorld::~GameWorld() noexcept = default;

void GameWorld::Update(float deltaSeconds)
{
	m_editCommands->Apply(m_generation, *m_state, *m_resources);
	if (m_cameraInputIntent.SpeedStepCount != 0.0f)
	{
		m_cameraNavigationSettings.MoveSpeedMetersPerSecond = CameraNavigation::ApplySpeedSteps(
		    m_cameraNavigationSettings.MoveSpeedMetersPerSecond,
		    m_cameraInputIntent.SpeedStepCount,
		    m_cameraNavigationSettings.MinimumMoveSpeedMetersPerSecond,
		    m_cameraNavigationSettings.MaximumMoveSpeedMetersPerSecond);
	}

	const ECS::GameWorldSystemExecutionContext executionContext{
	    .Resources = *m_resources,
	    .Executor = m_taskExecutor,
	    .Camera =
	        {
	            .Intent = m_cameraInputIntent,
	            .NavigationSettings = m_cameraNavigationSettings,
	            .DeltaSeconds = deltaSeconds,
	        },
	};

	if (!m_state->ExecuteSystems(executionContext))
	{
		Diagnostics::Fatal(g_gameWorldLogger, __FILE__, __LINE__, "Game-system graph execution failed.");
	}
	m_cameraInputIntent.LookDeltaX = 0.0f;
	m_cameraInputIntent.LookDeltaY = 0.0f;
	m_cameraInputIntent.SpeedStepCount = 0.0f;
}

WorldEditResult GameWorld::SubmitEdit(WorldEditCommand command, std::uint64_t expectedGeneration)
{
	return m_editCommands->Submit(std::move(command), expectedGeneration, m_generation, *m_state, *m_resources);
}

void GameWorld::PublishCameraInputIntent(const CameraInputIntent& intent) noexcept
{
	m_cameraInputIntent = intent;
}

void GameWorld::CommitSceneLoadPackage(Assets::SceneLoadPackage package)
{
	GameWorld stagedWorld(m_taskExecutor);
	InitializeSceneLoadLevel(*stagedWorld.m_state, package.Level);

	PopulateStagedScene(*stagedWorld.m_state, *stagedWorld.m_resources, package);

	m_state.swap(stagedWorld.m_state);
	m_resources.swap(stagedWorld.m_resources);
	++m_generation;
}

RenderFrameSubmission GameWorld::ExtractRenderFrameSubmission(std::uint64_t frameId)
{
	return m_renderFrameSubmissionExtractor->Extract(*m_state, *m_resources, AcquireReadView(), m_generation, frameId);
}

void GameWorld::FinalizeSceneLoadCommit()
{
	m_state->CommitDerivedStateAndPublish();
}

WorldReadView GameWorld::AcquireReadView() const noexcept
{
	return m_state->AcquireReadView();
}

WorldMaterialVariantView GameWorld::CaptureMaterialVariants() const
{
	WorldMaterialVariantView view;
	view.Active = m_resources->MaterialVariants.GetActive();
	view.Names.reserve(m_resources->MaterialVariants.GetCount());
	for (std::size_t index = 0; index < m_resources->MaterialVariants.GetCount(); ++index)
	{
		view.Names.emplace_back(m_resources->MaterialVariants.GetName(index));
	}
	return view;
}
