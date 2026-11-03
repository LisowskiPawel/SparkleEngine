#pragma once

#include "GameFramework/Public/GameFrameworkAPI.h"
#include "GameFramework/Public/Scene/Camera/CameraInputIntent.h"
#include "GameFramework/Public/Scene/Camera/CameraNavigationSettings.h"
#include "GameFramework/Public/World/WorldChange.h"
#include "GameFramework/Public/World/WorldReadView.h"
#include "GameFramework/Public/Rendering/RenderFrameSubmission.h"
#include "GameFramework/Public/World/WorldEditCommand.h"
#include "GameFramework/Public/World/WorldMaterialVariantView.h"

#include <cstdint>
#include <memory>

namespace ECS
{
	class GameWorldState;
	class RenderFrameSubmissionExtractor;
}

namespace Assets
{
	struct SceneLoadPackage;
}

struct GameWorldResourceStores;
class WorldEditCommandQueue;
class TaskExecutor;

class SPARKLE_ENGINE_API GameWorld final
{
public:
	explicit GameWorld(TaskExecutor& taskExecutor);
	~GameWorld() noexcept;

	GameWorld(const GameWorld&) = delete;
	GameWorld& operator=(const GameWorld&) = delete;
	GameWorld(GameWorld&&) = delete;
	GameWorld& operator=(GameWorld&&) = delete;

	void Update(float deltaSeconds);
	RenderFrameSubmission ExtractRenderFrameSubmission(std::uint64_t frameId);
	WorldReadView AcquireReadView() const noexcept;
	WorldChangeBatch ReadChanges(const WorldChangeCursor& cursor) const;
	bool AcknowledgeChanges(WorldChangeCursor& cursor, WorldSequence sequence) const noexcept;

	std::uint64_t GetGeneration() const noexcept { return m_generation; }

	void PublishCameraInputIntent(const CameraInputIntent& intent) noexcept;
	WorldMaterialVariantView CaptureMaterialVariants() const;
	WorldEditResult SubmitEdit(WorldEditCommand command, std::uint64_t expectedGeneration);

private:
	friend class LevelSession;
	void CommitSceneLoadPackage(Assets::SceneLoadPackage package);
	void FinalizeSceneLoadCommit();

	std::unique_ptr<ECS::GameWorldState> m_state;
	std::unique_ptr<GameWorldResourceStores> m_resources;
	TaskExecutor& m_taskExecutor;
	CameraInputIntent m_cameraInputIntent;
	CameraNavigationSettings m_cameraNavigationSettings;
	std::uint64_t m_generation = 1;
	std::unique_ptr<WorldEditCommandQueue> m_editCommands;
	std::unique_ptr<ECS::RenderFrameSubmissionExtractor> m_renderFrameSubmissionExtractor;
};
