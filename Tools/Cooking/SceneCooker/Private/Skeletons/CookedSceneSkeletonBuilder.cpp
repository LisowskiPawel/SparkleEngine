#include "PCH.h"
#include "GameFramework/Public/Assets/Cooked/CookedSceneMetadataRecords.h"

#include "Skeletons/CookedSceneSkeletonBuilder.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Hash/HashUtils.h"
#include "GameFramework/Public/Scene/Animations/SkeletonTransformContract.h"

#include <cstring>
#include <cmath>
#include <cstdint>
#include <format>
#include <limits>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace CookedSceneSkeletonBuilder
{
	static Assets::CookedAssetId BuildSkeletonAssetId(std::string_view sceneAssetId, std::uint32_t sourceSkinIndex)
	{
		return Hash::Fnv1a64(std::string(sceneAssetId) + "#skeleton#" + std::to_string(sourceSkinIndex));
	}

	static void CopyJointName(std::string_view sourceName, char (&destination)[64]) noexcept
	{
		std::memset(destination, 0, sizeof(destination));
		std::memcpy(destination, sourceName.data(), sourceName.size());
	}

	static std::uint32_t AppendEvaluationTransform(const DirectX::XMFLOAT4X4& matrix, std::uint32_t parent, CookedSkeletonAssetBuild& asset)
	{
		if (asset.evaluationJoints.size() >= Assets::kMaxAnimationEvaluationJoints)
		{
			throw Diagnostics::Error("Animation preview exceeds 1,024 evaluation joints including parent helpers; simplify the exported rig.");
		}
		if (!SkeletonTransformContract::IsTrsDecomposable(matrix))
		{
			throw Diagnostics::Error("Animation preview cannot represent a sheared parent transform; bake transforms in the source asset before export.");
		}

		DirectX::XMVECTOR scale;
		DirectX::XMVECTOR rotation;
		DirectX::XMVECTOR translation;
		const auto transform = DirectX::XMLoadFloat4x4(&matrix);
		if (!DirectX::XMMatrixDecompose(&scale, &rotation, &translation, transform))
		{
			throw Diagnostics::Error("Animation preview transform decomposition failed; bake the source rig.");
		}
		Assets::CookedSkeletonEvaluationJointRecord record{.parentJointIndex = parent};
		DirectX::XMStoreFloat3(&record.translation, translation);
		DirectX::XMStoreFloat4(&record.rotation, DirectX::XMQuaternionNormalize(rotation));
		DirectX::XMStoreFloat3(&record.scale, scale);
		const auto normalizedRotation = DirectX::XMLoadFloat4(&record.rotation);
		const auto reconstructed = DirectX::XMMatrixAffineTransformation(scale, DirectX::XMVectorZero(), normalizedRotation, translation);
		DirectX::XMFLOAT4X4 reconstructedValues;
		DirectX::XMStoreFloat4x4(&reconstructedValues, reconstructed);
		for (std::uint32_t row = 0; row < 4; ++row)
		{
			for (std::uint32_t column = 0; column < 4; ++column)
			{
				const float expected = matrix.m[row][column];
				const float error = std::abs(reconstructedValues.m[row][column] - expected);
				if (error > 1.0e-5f + 1.0e-5f * std::abs(expected))
				{
					throw Diagnostics::Error("Animation preview would lose shear or non-TRS bind data; bake the source transforms before export.");
				}
			}
		}
		const float components[] = {record.translation.x, record.translation.y, record.translation.z, record.scale.x, record.scale.y, record.scale.z};
		for (float component : components)
		{
			if (!std::isfinite(component) || std::abs(component) > 65504.0f)
			{
				throw Diagnostics::Error("Animation preview bind transform exceeds the skeletal numeric range; rescale the source rig.");
			}
		}
		const auto index = static_cast<std::uint32_t>(asset.evaluationJoints.size());
		asset.evaluationJoints.push_back(record);
		return index;
	}

	static void AppendEvaluationBranch(const ImportedSkeleton& skeleton, std::uint32_t jointIndex, std::uint32_t parent, CookedSkeletonAssetBuild& asset)
	{
		const auto& joint = skeleton.joints[jointIndex];
		const DirectX::XMFLOAT4X4 identity{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
		if (std::memcmp(&joint.parentSpaceTransform, &identity, sizeof(identity)) != 0)
		{
			parent = AppendEvaluationTransform(joint.parentSpaceTransform, parent, asset);
		}
		const auto evaluationIndex = AppendEvaluationTransform(joint.bindLocalTransform, parent, asset);
		asset.joints[jointIndex].evaluationJointIndex = evaluationIndex;
		for (std::uint32_t child = 0; child < skeleton.joints.size(); ++child)
		{
			if (skeleton.joints[child].parentJointIndex == jointIndex)
			{
				AppendEvaluationBranch(skeleton, child, evaluationIndex, asset);
			}
		}
	}

	void BuildSkeletons(const SourceImportOutput& importOutput, std::string_view sceneAssetId, CookedSceneBuild& outBuild)
	{
		outBuild.outputs.skeletonAssets.clear();
		outBuild.manifest.skeletonRefs.clear();
		outBuild.outputs.skeletonAssets.reserve(importOutput.scene.skeletons.size());
		outBuild.manifest.skeletonRefs.reserve(importOutput.scene.skeletons.size());

		std::unordered_set<std::uint32_t> sourceSkinIndices;
		for (std::size_t skeletonIndex = 0; skeletonIndex < importOutput.scene.skeletons.size(); ++skeletonIndex)
		{
			const ImportedSkeleton& importedSkeleton = importOutput.scene.skeletons[skeletonIndex];
			if (importedSkeleton.joints.size() > Assets::kMaxAnimationEvaluationJoints)
			{
				throw Diagnostics::Error("Animation preview exceeds 1,024 joints before parent helper expansion; simplify the exported rig.");
			}
			if (!importedSkeleton.IsValid())
			{
				throw Diagnostics::Error(std::format("Imported skeleton {} has no joints.", skeletonIndex));
			}
			if (!sourceSkinIndices.insert(importedSkeleton.sourceSkinIndex).second)
			{
				throw Diagnostics::Error(std::format("Imported skeleton {} duplicates source skin index {}.", skeletonIndex, importedSkeleton.sourceSkinIndex));
			}

			std::vector<std::uint32_t> evaluationOrder;
			if (!SkeletonTransformContract::BuildEvaluationOrder(importedSkeleton.joints, evaluationOrder))
			{
				throw Diagnostics::Error(std::format("Imported skeleton {} has a cyclic joint hierarchy.", skeletonIndex));
			}

			CookedSkeletonAssetBuild skeletonAsset;
			skeletonAsset.assetId = BuildSkeletonAssetId(sceneAssetId, importedSkeleton.sourceSkinIndex);
			skeletonAsset.sourceSkinIndex = importedSkeleton.sourceSkinIndex;
			skeletonAsset.sourcePath = importOutput.GetSourcePath();
			skeletonAsset.joints.reserve(importedSkeleton.joints.size());

			std::unordered_set<std::uint32_t> sourceNodeIndices;
			for (std::size_t jointIndex = 0; jointIndex < importedSkeleton.joints.size(); ++jointIndex)
			{
				const ImportedJoint& importedJoint = importedSkeleton.joints[jointIndex];

				const bool bJointIdentityValid = importedJoint.name.size() < sizeof(Assets::CookedSkeletonJointRecord::name)
				    && importedJoint.sourceNodeIndex != (std::numeric_limits<std::uint32_t>::max)();

				const bool bSourceNodeUnique = bJointIdentityValid && sourceNodeIndices.insert(importedJoint.sourceNodeIndex).second;
				const bool bRootJoint = importedJoint.parentJointIndex == (std::numeric_limits<std::uint32_t>::max)();
				const bool bDistinctParent = importedJoint.parentJointIndex < importedSkeleton.joints.size() && importedJoint.parentJointIndex != jointIndex;
				const bool bParentValid = bRootJoint || bDistinctParent;
				const bool bJointValid = bSourceNodeUnique && bParentValid;

				if (!bJointValid)
				{
					throw Diagnostics::Error(std::format("Imported skeleton {} has invalid joint {}.", skeletonIndex, jointIndex));
				}

				const DirectX::XMFLOAT4X4* parentBindModel = importedJoint.parentJointIndex < importedSkeleton.joints.size()
				    ? &importedSkeleton.joints[importedJoint.parentJointIndex].bindModelTransform
				    : nullptr;

				const bool bInverseBindFinite = SkeletonTransformContract::IsFinite(importedJoint.inverseBindMatrix);
				const bool bLocalBindFinite = bInverseBindFinite && SkeletonTransformContract::IsFinite(importedJoint.bindLocalTransform);
				const bool bParentSpaceFinite = bLocalBindFinite && SkeletonTransformContract::IsFinite(importedJoint.parentSpaceTransform);
				const bool bModelBindFinite = bParentSpaceFinite && SkeletonTransformContract::IsFinite(importedJoint.bindModelTransform);
				const bool bInverseBindInvertible = bModelBindFinite && SkeletonTransformContract::IsInvertible(importedJoint.inverseBindMatrix);
				const bool bLocalBindInvertible = bInverseBindInvertible && SkeletonTransformContract::IsInvertible(importedJoint.bindLocalTransform);
				const bool bLocalBindDecomposable = bLocalBindInvertible && SkeletonTransformContract::IsTrsDecomposable(importedJoint.bindLocalTransform);

				const bool bBindInvariantSatisfied = bLocalBindDecomposable
				    && SkeletonTransformContract::SatisfiesBindInvariant(importedJoint.bindLocalTransform, importedJoint.parentSpaceTransform, parentBindModel, importedJoint.bindModelTransform);

				if (!bBindInvariantSatisfied)
				{
					throw Diagnostics::Error(std::format("Imported skeleton {} joint {} violates the canonical transform contract.", skeletonIndex, jointIndex));
				}

				Assets::CookedSkeletonJointRecord jointRecord;
				CopyJointName(importedJoint.name, jointRecord.name);
				jointRecord.sourceNodeIndex = importedJoint.sourceNodeIndex;
				jointRecord.inverseBindMatrix = importedJoint.inverseBindMatrix;
				skeletonAsset.joints.push_back(jointRecord);
			}
			for (std::uint32_t joint = 0; joint < importedSkeleton.joints.size(); ++joint)
			{
				if (importedSkeleton.joints[joint].parentJointIndex == Assets::kInvalidCookedSkeletonJointIndex)
				{
					AppendEvaluationBranch(importedSkeleton, joint, Assets::kInvalidCookedSkeletonJointIndex, skeletonAsset);
				}
			}

			outBuild.manifest.skeletonRefs.push_back(Assets::CookedSceneSkeletonRef{.skeletonAssetId = skeletonAsset.assetId, .sourceSkinIndex = importedSkeleton.sourceSkinIndex, .flags = 0});
			outBuild.outputs.skeletonAssets.push_back(std::move(skeletonAsset));
		}
	}
}
