#pragma once

#include "Animation/AnimationOutputStorage.h"
#include "Animation/AnimationClipResource.h"
#include "GameFramework/Public/Scene/Camera/CameraDesc.h"
#include "GameFramework/Public/Scene/Camera/SceneCameraEntry.h"
#include "GameFramework/Public/Scene/Lighting/SceneLightDesc.h"
#include "GameFramework/Public/Scene/Transform.h"
#include "GameFramework/Public/World/SkyEnvironment.h"
#include "GameFramework/Public/World/WorldReadView.h"
#include "World/ECS/EntityRegistry.h"
#include "World/Extraction/WorldExtractionStorage.h"
#include "World/Resources/MorphWeightStorage.h"
#include "World/Resources/MeshResourceStore.h"
#include "World/SceneMeshInstanceData.h"
#include "World/Systems/GameSystemGraph.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include <atomic>
#include <memory>
#include <optional>

class SkeletonResourceStore;
class GameWorldResourceStores;
class TaskExecutor;

namespace ECS
{
	class RenderFrameSubmissionExtractor;
	class GameWorldSystemExecution;
	struct GameWorldSystemExecutionContext;

	class GameWorldState final
	{
	public:
		GameWorldState();

		bool IsAlive(EntityId entity) const noexcept { return m_registry.IsAlive(entity); }

		std::size_t GetEntityCount() const noexcept { return m_registry.GetLiveCount(); }

		EntityId AddCamera(SceneCameraEntry&& entry, bool bActive = false);
		std::size_t GetCameraCount() const noexcept;
		EntityId GetCameraEntity(std::size_t index) const noexcept;
		bool IsCamera(EntityId entity) const noexcept;
		std::optional<SceneCameraEntry> ReadCamera(EntityId entity) const;
		bool SetActiveCamera(EntityId entity) noexcept;

		EntityId GetActiveCamera() const noexcept { return m_activeCamera; }

		bool WriteCameraDesc(EntityId entity, const CameraDesc& desc) noexcept;
		bool WriteTransform(EntityId entity, const Transform& transform) noexcept;
		Transform ReadTransform(EntityId entity) const noexcept;
		bool WriteVisibility(EntityId entity, bool bVisible) noexcept;
		bool ReadVisibility(EntityId entity) const noexcept;

		EntityId AddMesh(SceneMeshInstanceData&& instance);
		std::span<const EntityId> GetMeshEntities() const noexcept;
		bool WriteMeshMaterial(EntityId entity, MaterialHandle material) noexcept;
		void AppendMeshInstanceGroups(std::vector<SceneMeshInstanceGroupData> groups);

		std::size_t GetMeshInstanceGroupCount() const noexcept { return m_meshInstanceGroups.size(); }

		std::span<const WorldExtractionStorage::MeshSlot> GetExtractedMeshes() const noexcept { return m_extraction.GetExtractedMeshes(); }

		std::span<const SceneMeshInstanceGroupData> GetExtractedMeshGroups() const noexcept { return m_extraction.GetMeshGroups(); }

		EntityId AddLight(SceneLightDesc&& desc);
		std::optional<SceneLightDesc> ReadLight(EntityId entity) const;
		bool WriteLight(EntityId entity, SceneLightDesc&& desc);

		void AppendAnimationClips(std::vector<AnimationClipResource> clips, AnimationClipResourceStore& resources, std::uint64_t sourceInstanceId);
		bool PrepareSystemResources(GameWorldResourceStores& resources);
		bool ExecuteSystems(const GameWorldSystemExecutionContext& context);

		std::optional<SkyEnvironment> ReadSkyEnvironment() const { return m_skyEnvironment; }

		void WriteSkyEnvironment(SkyEnvironment environment) noexcept;
		void RemoveSkyEnvironment() noexcept;
		void NotifyResourceChanged() noexcept;

		void CommitDerivedStateAndPublish();
		WorldReadView AcquireReadView() const noexcept;

	private:
		static constexpr std::size_t MaximumPendingChanges = 4096;

		friend class RenderFrameSubmissionExtractor;
		friend class GameWorldSystemExecution;

		struct SystemExecutionArena final
		{
			std::vector<EntityId> CameraChanges;
			std::vector<EntityId> AnimationChanges;
			std::vector<EntityId> MorphChanges;
			std::vector<EntityId> DirtyTransforms;
			std::vector<EntityId> EvaluatedTransforms;
			std::vector<EntityId> CameraDerivedChanges;
		};

		void MarkTransformDirty(EntityId entity) noexcept;
		bool AddMeshComponents(EntityId entity, const SceneMeshInstanceData& instance, MeshResourceHandle resource);
		bool AddMeshDeformation(EntityId entity, const SceneMeshInstanceData& instance, AnimationOutputSlotHandle weights);
		bool PrepareSystemExecution(GameWorldResourceStores& resources);
		void PrepareSystemArena();
		bool CommitSystemOutputs();
		bool CommitExtraction();
		void RecordSystemChanges(std::vector<EntityId>& changes);
		void RecordTransformChanges(std::vector<EntityId>& changes);
		void RecordChange(EntityId entity) noexcept;
		std::span<const EntityId> PrepareDirtyTransforms();
		void CommitDirtyTransforms(std::span<const EntityId> entities);
		void PublishPendingChanges();
		void PublishReadView(std::span<const EntityId> changes, WorldSequence sequence, bool bFullBaseline);
		void BuildReadViewBaseline(WorldReadView::Storage& next) const;
		void ApplyReadViewChanges(WorldReadView::Storage& next, std::span<const EntityId> changes) const;
		void RefreshReadViewEntity(WorldReadView::Storage& next, EntityId entity) const;
		WorldCameraReadData BuildCameraReadData(EntityId entity) const;
		WorldLightReadData BuildLightReadData(EntityId entity) const;
		WorldMeshReadData BuildMeshReadData(EntityId entity) const;

		template <typename T> std::size_t Count() const noexcept { return m_registry.GetStorage<T>().GetEntities().size(); }

		template <typename T> EntityId EntityAt(std::size_t index) const noexcept
		{
			const std::span<const EntityId> entities = m_registry.GetStorage<T>().GetEntities();
			return index < entities.size() ? entities[index] : EntityId::Invalid();
		}

		EntityRegistry m_registry;
		MeshResourceStore m_meshResources;
		AnimationOutputStorage m_animationOutput;
		MorphWeightStorage m_morphWeights;
		WorldExtractionStorage m_extraction;
		SystemExecutionArena m_systemArena;
		CompiledGameSystemGraph m_systemGraph;
		std::vector<SceneMeshInstanceGroupData> m_meshInstanceGroups;
		std::optional<SkyEnvironment> m_skyEnvironment;
		std::vector<EntityId> m_dirtyTransforms;
		std::vector<EntityId> m_pendingChanges;
		WorldSequence m_readSequence = 0;
		std::atomic<std::shared_ptr<const WorldReadView::Storage>> m_publishedReadView;
		std::uint64_t m_readGeneration = 0;
		bool m_bForceFullPublication = true;
		bool m_bEvaluateAllTransforms = false;
		bool m_bChangesOverflowed = false;
		EntityId m_activeCamera;
		std::uint64_t m_nextCameraIdentity = 0;
		std::uint64_t m_nextLightIdentity = 0;
	};
}
