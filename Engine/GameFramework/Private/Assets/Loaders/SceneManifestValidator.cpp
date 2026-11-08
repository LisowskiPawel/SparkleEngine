#include "PCH.h"

#include "Assets/Loaders/SceneManifestValidator.h"

#include "Assets/Cooked/LoadedSceneManifest.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <limits>

namespace Assets
{
	template <std::size_t Capacity> static bool HasTerminatedName(const char (&name)[Capacity]) noexcept
	{
		return Strings::IsNullTerminated(std::span(name));
	}

	template <std::size_t Capacity> static bool HasValidName(const char (&name)[Capacity]) noexcept
	{
		return name[0] != '\0' && HasTerminatedName(name);
	}

	[[noreturn]] static void Invalid(const std::string& message)
	{
		throw Diagnostics::Error(message);
	}

	static bool HasFeatureFlag(std::uint32_t flags, CookedSceneFeatureFlags feature) noexcept
	{
		return (flags & ToCookedSceneFeatureFlagMask(feature)) != 0u;
	}

	static void ValidateFeatures(const LoadedSceneManifest& manifest)
	{
		constexpr std::uint32_t knownFeatureFlags = ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::Cameras) | ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::Lights)
		    | ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::Skeletons) | ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::Animations)
		    | ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::SkinnedMeshes) | ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::MorphTargets)
		    | ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::MaterialVariants) | ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::AuthoredMeshInstancing);

		if ((manifest.header.featureFlags & ~knownFeatureFlags) != 0u)
		{
			Invalid(std::format("Cooked scene manifest uses unknown feature flag bits 0x{:08X}", manifest.header.featureFlags));
		}

		if (!manifest.cameras.empty() && !HasFeatureFlag(manifest.header.featureFlags, CookedSceneFeatureFlags::Cameras))
		{
			Invalid("Cooked scene manifest has camera records but is missing the Cameras feature flag");
		}

		if (!manifest.lights.empty() && !HasFeatureFlag(manifest.header.featureFlags, CookedSceneFeatureFlags::Lights))
		{
			Invalid("Cooked scene manifest has light records but is missing the Lights feature flag");
		}

		if (!manifest.skeletonRefs.empty() && !HasFeatureFlag(manifest.header.featureFlags, CookedSceneFeatureFlags::Skeletons))
		{
			Invalid("Cooked scene manifest has skeleton refs but is missing the Skeletons feature flag");
		}

		const bool bAnimationFeatureMissing = !manifest.animationReferences.empty() && !HasFeatureFlag(manifest.header.featureFlags, CookedSceneFeatureFlags::Animations);

		if (bAnimationFeatureMissing)
		{
			Invalid("Cooked scene manifest has animation refs but is missing the Animations feature flag");
		}

		const bool bFeatureMissingVariantMappings = !manifest.materialVariants.empty() || !manifest.materialVariantMappings.empty();
		const bool bMaterialVariantFeatureMissing = bFeatureMissingVariantMappings && (!HasFeatureFlag(manifest.header.featureFlags, CookedSceneFeatureFlags::MaterialVariants));

		if (bMaterialVariantFeatureMissing)
		{
			Invalid("Cooked scene manifest has material variant records but is missing the MaterialVariants feature flag");
		}
	}

	static void ValidateMeshReferences(const LoadedSceneManifest& manifest)
	{
		for (std::size_t meshReferenceIndex = 0; meshReferenceIndex < manifest.meshAssetReferences.size(); ++meshReferenceIndex)
		{
			if (manifest.meshAssetReferences[meshReferenceIndex].meshAssetId == InvalidCookedAssetId)
			{
				Invalid(std::format("Cooked scene manifest has an invalid mesh asset reference at index {}", meshReferenceIndex));
			}
			if (manifest.meshAssetReferences[meshReferenceIndex].meshAssetKind != CookedMeshAssetKind::Static
			    && manifest.meshAssetReferences[meshReferenceIndex].meshAssetKind != CookedMeshAssetKind::Skeletal)
			{
				Invalid(std::format("Cooked scene manifest has an invalid mesh asset kind at index {}", meshReferenceIndex));
			}
		}
	}

	static void ValidateInstanceReferenceRanges(const LoadedSceneManifest& manifest, std::size_t instanceIndex)
	{
		const CookedSceneInstanceRecord& instance = manifest.instances[instanceIndex];
		if (instance.meshAssetIndex >= manifest.meshAssetReferences.size())
		{
			Invalid(std::format("Cooked scene instance {} references mesh asset index {} but only {} mesh assets exist", instanceIndex, instance.meshAssetIndex, manifest.meshAssetReferences.size()));
		}

		const bool bInvalidInstanceMaterial = instance.materialAssetIndex != kInvalidCookedMaterialAssetIndex && instance.materialAssetIndex >= manifest.materialAssetReferences.size();

		if (bInvalidInstanceMaterial)
		{
			Invalid(
			    std::format(
			        "Cooked scene instance {} references material asset index {} but only {} material assets exist",
			        instanceIndex,
			        instance.materialAssetIndex,
			        manifest.materialAssetReferences.size()));
		}

		if (instance.groupIndex != kInvalidCookedSceneInstanceGroupIndex && instance.groupIndex >= manifest.instanceGroups.size())
		{
			Invalid(std::format("Cooked scene instance {} references instance group index {} but only {} groups exist", instanceIndex, instance.groupIndex, manifest.instanceGroups.size()));
		}

		const bool bInvalidInstanceSkeleton = instance.skeletonRefIndex != kInvalidCookedSceneSkeletonRefIndex && instance.skeletonRefIndex >= manifest.skeletonRefs.size();

		if (bInvalidInstanceSkeleton)
		{
			Invalid(std::format("Cooked scene instance {} references skeleton ref index {} but only {} skeleton refs exist", instanceIndex, instance.skeletonRefIndex, manifest.skeletonRefs.size()));
		}

		if (instance.firstMorphWeight != kInvalidCookedSceneMorphWeightIndex)
		{
			const bool bInvalidWeightRangeMorphWeights = instance.morphWeightCount == 0u || instance.firstMorphWeight >= manifest.morphWeights.size();
			const bool bInvalidMorphWeightRange = bInvalidWeightRangeMorphWeights || (instance.morphWeightCount > manifest.morphWeights.size() - instance.firstMorphWeight);

			if (bInvalidMorphWeightRange)
			{
				Invalid(
				    std::format(
				        "Cooked scene instance {} references invalid morph weight range first={} count={} with {} weights",
				        instanceIndex,
				        instance.firstMorphWeight,
				        instance.morphWeightCount,
				        manifest.morphWeights.size()));
			}
		}
		else if (instance.morphWeightCount != 0u)
		{
			Invalid(std::format("Cooked scene instance {} has morph weights without a valid first weight index", instanceIndex));
		}
	}

	static void ValidateInstanceMeshBinding(const LoadedSceneManifest& manifest, std::size_t instanceIndex)
	{
		const CookedSceneInstanceRecord& instance = manifest.instances[instanceIndex];
		const CookedSceneMeshAssetRef& meshReference = manifest.meshAssetReferences[instance.meshAssetIndex];
		if (meshReference.meshAssetKind == CookedMeshAssetKind::Skeletal && instance.skeletonRefIndex == kInvalidCookedSceneSkeletonRefIndex)
		{
			Invalid(std::format("Cooked scene instance {} uses a skeletal mesh without a skeleton ref", instanceIndex));
		}
		if (meshReference.meshAssetKind == CookedMeshAssetKind::Static && instance.skeletonRefIndex != kInvalidCookedSceneSkeletonRefIndex)
		{
			Invalid(std::format("Cooked scene instance {} uses a static mesh with a skeleton ref", instanceIndex));
		}
	}

	static void ValidateInstances(const LoadedSceneManifest& manifest)
	{
		for (std::size_t instanceIndex = 0; instanceIndex < manifest.instances.size(); ++instanceIndex)
		{
			ValidateInstanceReferenceRanges(manifest, instanceIndex);
			ValidateInstanceMeshBinding(manifest, instanceIndex);
		}
	}

	static void ValidateInstanceGroupReferences(const LoadedSceneManifest& manifest, std::size_t groupIndex)
	{
		const CookedSceneInstanceGroupRecord& group = manifest.instanceGroups[groupIndex];
		if (group.meshAssetIndex >= manifest.meshAssetReferences.size())
		{
			Invalid(std::format("Cooked scene instance group {} references mesh asset index {} but only {} mesh assets exist", groupIndex, group.meshAssetIndex, manifest.meshAssetReferences.size()));
		}

		const bool bInvalidGroupMaterial = group.materialAssetIndex != kInvalidCookedMaterialAssetIndex && group.materialAssetIndex >= manifest.materialAssetReferences.size();

		if (bInvalidGroupMaterial)
		{
			Invalid(
			    std::format(
			        "Cooked scene instance group {} references material asset index {} but only {} material assets exist",
			        groupIndex,
			        group.materialAssetIndex,
			        manifest.materialAssetReferences.size()));
		}
	}

	static void ValidateInstanceGroupRange(const LoadedSceneManifest& manifest, std::size_t groupIndex)
	{
		const CookedSceneInstanceGroupRecord& group = manifest.instanceGroups[groupIndex];
		const bool bInvalidInstanceRangeInstances = group.instanceCount == 0 || group.firstInstance >= manifest.instances.size();
		const bool bInvalidGroupInstanceRange = bInvalidInstanceRangeInstances || (group.instanceCount > manifest.instances.size() - group.firstInstance);

		if (bInvalidGroupInstanceRange)
		{
			Invalid(
			    std::format(
			        "Cooked scene instance group {} references invalid instance range first={} count={} with {} instances",
			        groupIndex,
			        group.firstInstance,
			        group.instanceCount,
			        manifest.instances.size()));
		}

		for (std::uint32_t instanceOffset = 0; instanceOffset < group.instanceCount; ++instanceOffset)
		{
			const std::size_t instanceIndex = static_cast<std::size_t>(group.firstInstance) + instanceOffset;
			if (manifest.instances[instanceIndex].groupIndex != groupIndex)
			{
				Invalid(
				    std::format("Cooked scene instance group {} range contains instance {} with mismatched group index {}", groupIndex, instanceIndex, manifest.instances[instanceIndex].groupIndex));
			}
		}
	}

	static void ValidateInstanceGroupKind(const LoadedSceneManifest& manifest, std::size_t groupIndex)
	{
		const CookedSceneInstanceGroupRecord& group = manifest.instanceGroups[groupIndex];
		if (group.groupKind != CookedSceneInstanceGroupKind::None && group.groupKind != CookedSceneInstanceGroupKind::SharedMeshReference
		    && group.groupKind != CookedSceneInstanceGroupKind::AuthoredInstanceGroup)
		{
			Invalid(std::format("Cooked scene instance group {} uses an unknown group kind", groupIndex));
		}
	}

	static void ValidateInstanceGroups(const LoadedSceneManifest& manifest)
	{
		for (std::size_t groupIndex = 0; groupIndex < manifest.instanceGroups.size(); ++groupIndex)
		{
			ValidateInstanceGroupReferences(manifest, groupIndex);
			ValidateInstanceGroupRange(manifest, groupIndex);
			ValidateInstanceGroupKind(manifest, groupIndex);
		}
	}

	static void ValidateCameras(const LoadedSceneManifest& manifest)
	{
		for (std::size_t cameraIndex = 0; cameraIndex < manifest.cameras.size(); ++cameraIndex)
		{
			const CookedSceneCameraRecord& camera = manifest.cameras[cameraIndex];
			const bool bInvalidCameraRecordProjectionKind = !HasTerminatedName(camera.name) || camera.projectionKind != CookedSceneCameraProjectionKind::Perspective;
			const bool bInvalidCameraRecordNodeIndex = bInvalidCameraRecordProjectionKind || (camera.sourceNodeIndex == (std::numeric_limits<std::uint32_t>::max)());
			const bool bInvalidCameraRecord = bInvalidCameraRecordNodeIndex || (camera.flags != 0u);

			if (bInvalidCameraRecord)
			{
				Invalid(std::format("Cooked scene camera {} has an unsupported or invalid projection", cameraIndex));
			}
		}
	}

	static void ValidateLights(const LoadedSceneManifest& manifest)
	{
		for (std::size_t lightIndex = 0; lightIndex < manifest.lights.size(); ++lightIndex)
		{
			const CookedSceneLightRecord& light = manifest.lights[lightIndex];
			const bool bInvalidLightRecordName = !HasTerminatedName(light.name);
			const bool bInvalidLightRecordKind = light.kind != CookedSceneLightKind::Directional && light.kind != CookedSceneLightKind::Point;
			const bool bNotPunctual = bInvalidLightRecordKind && (light.kind != CookedSceneLightKind::Spot);
			const bool bUnknownLightKind = bNotPunctual && (light.kind != CookedSceneLightKind::Rect);
			const bool bInvalidLightRecord = bInvalidLightRecordName || bUnknownLightKind;

			if (bInvalidLightRecord)
			{
				Invalid(std::format("Cooked scene light {} uses an unknown light kind", lightIndex));
			}

			if (light.sourceNodeIndex == kInvalidCookedSceneLightSourceNodeIndex || (light.flags & ~1u) != 0u)
			{
				Invalid(std::format("Cooked scene light {} has invalid structural metadata", lightIndex));
			}
		}
	}

	static void ValidateSkeletonRefs(const LoadedSceneManifest& manifest)
	{
		for (std::size_t skeletonIndex = 0; skeletonIndex < manifest.skeletonRefs.size(); ++skeletonIndex)
		{
			if (manifest.skeletonRefs[skeletonIndex].skeletonAssetId == InvalidCookedAssetId)
			{
				Invalid(std::format("Cooked scene skeleton ref {} has an invalid asset id", skeletonIndex));
			}
		}
	}

	static void ValidateAnimationRefs(const LoadedSceneManifest& manifest)
	{
		for (std::size_t animationIndex = 0; animationIndex < manifest.animationReferences.size(); ++animationIndex)
		{
			if (manifest.animationReferences[animationIndex].animationAssetId == InvalidCookedAssetId)
			{
				Invalid(std::format("Cooked scene animation ref {} has an invalid asset id", animationIndex));
			}
		}
	}

	static void ValidateMetadata(const LoadedSceneManifest& manifest)
	{
		ValidateCameras(manifest);
		ValidateLights(manifest);
		ValidateSkeletonRefs(manifest);
		ValidateAnimationRefs(manifest);
	}

	static void ValidateMaterialVariants(const LoadedSceneManifest& manifest)
	{
		for (std::size_t variantIndex = 0; variantIndex < manifest.materialVariants.size(); ++variantIndex)
		{
			if (!HasValidName(manifest.materialVariants[variantIndex].name))
			{
				Invalid(std::format("Cooked scene material variant {} has an invalid name", variantIndex));
			}
		}

		for (std::size_t mappingIndex = 0; mappingIndex < manifest.materialVariantMappings.size(); ++mappingIndex)
		{
			const CookedSceneMaterialVariantMappingRecord& mapping = manifest.materialVariantMappings[mappingIndex];
			if (mapping.meshAssetIndex >= manifest.meshAssetReferences.size())
			{
				Invalid(std::format("Cooked scene material variant mapping {} references an invalid mesh asset", mappingIndex));
			}

			if (mapping.variantIndex >= manifest.materialVariants.size())
			{
				Invalid(std::format("Cooked scene material variant mapping {} references an invalid variant", mappingIndex));
			}

			if (mapping.materialAssetIndex >= manifest.materialAssetReferences.size())
			{
				Invalid(std::format("Cooked scene material variant mapping {} references an invalid material", mappingIndex));
			}
		}
	}

	void SceneManifestValidator::ValidateHeader(const LoadedSceneManifest& manifest)
	{
		if (!manifest.header.fileHeader.HasMagic(kCookedSceneManifestMagic))
		{
			Invalid("Invalid cooked scene manifest magic");
		}
	}

	void SceneManifestValidator::ValidateRecords(const LoadedSceneManifest& manifest)
	{
		ValidateFeatures(manifest);
		ValidateMeshReferences(manifest);
		ValidateInstances(manifest);
		ValidateInstanceGroups(manifest);
		ValidateMetadata(manifest);
		ValidateMaterialVariants(manifest);
	}
}
