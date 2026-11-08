#pragma once

#include "GameFramework/Public/Rendering/RenderSceneDynamicData.h"
#include "World/Extraction/WorldExtractionStorage.h"
#include "World/ECS/Components/AnimationComponents.h"
#include "World/ECS/ComponentStorage.h"

#include <cstdint>
#include <limits>
#include <span>
#include <vector>

class SkeletonResourceStore;
class WorldReadView;

namespace ECS
{
	class AnimationOutputStorage;
	class MorphWeightStorage;
	class RenderObjectDeltaExtractor;
	class RenderObjectIdentityMap;

	class RenderSceneDynamicDataExtractor final
	{
	public:
		void BeginScene() noexcept;
		void ExtractObjects(std::span<const WorldExtractionStorage::MeshSlot> meshes, const RenderObjectDeltaExtractor& objects, RenderSceneDynamicData& dynamic);
		static void ExtractSkinning(
		    const ComponentStorage<SkinningState>& skinningStates,
		    const ComponentStorage<AnimationState>& animationStates,
		    const AnimationOutputStorage& animation,
		    const SkeletonResourceStore& skeletons,
		    std::span<const WorldExtractionStorage::MeshSlot> meshes,
		    const RenderObjectDeltaExtractor& objects,
		    RenderSceneDynamicData& dynamic);
		void ExtractMorphWeights(
		    const ComponentStorage<MorphState>& morphStates,
		    const ComponentStorage<AnimationState>& animationStates,
		    const AnimationOutputStorage& animation,
		    const MorphWeightStorage& weights,
		    const RenderObjectDeltaExtractor& objects,
		    RenderSceneDynamicData& dynamic);
		static void ExtractLights(const WorldReadView& readView, RenderObjectIdentityMap& identities, RenderSceneDynamicData& dynamic);

	private:
		struct MorphMetadata final
		{
			EntityId Entity;
			RenderAnimationAssetHandle Animation;
			std::uint32_t TargetNodeIndex = (std::numeric_limits<std::uint32_t>::max)();
		};

		static bool HasSameObjectData(const RenderObjectDynamicData& left, const RenderObjectDynamicData& right) noexcept;
		void BuildMorphMetadata(const AnimationOutputStorage& animation, const ComponentStorage<AnimationState>& animationStates);
		void ExtractMorphWeightRanges(const ComponentStorage<MorphState>& morphStates, const MorphWeightStorage& weights, const RenderObjectDeltaExtractor& objects, RenderSceneDynamicData& dynamic);

		std::vector<RenderObjectDynamicData> m_publishedObjects;
		std::vector<RenderObjectDynamicData> m_currentObjects;
		std::vector<MorphMetadata> m_morphMetadata;
	};
}
