#include "PCH.h"

#include "Gltf/Animation/GltfAnimationImporter.h"
#include "Gltf/Animation/GltfAnimationSampler.h"
#include "Core/Public/Diagnostics/Error.h"

#include <cgltf.h>

#include <algorithm>
#include <format>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace GltfAnimationImporter
{
	static ImportedAnimationTargetPath ToImportedTargetPath(cgltf_animation_path_type path) noexcept
	{
		switch (path)
		{
			case cgltf_animation_path_type_translation:
				return ImportedAnimationTargetPath::Translation;
			case cgltf_animation_path_type_rotation:
				return ImportedAnimationTargetPath::Rotation;
			case cgltf_animation_path_type_scale:
				return ImportedAnimationTargetPath::Scale;
			case cgltf_animation_path_type_weights:
				return ImportedAnimationTargetPath::Weights;
			default:
				return ImportedAnimationTargetPath::Unknown;
		}
	}

	static std::uint32_t ResolveMorphWeightCount(const cgltf_node& targetNode) noexcept
	{
		if (targetNode.mesh == nullptr || targetNode.mesh->primitives_count == 0)
		{
			return 0;
		}
		for (cgltf_size primitiveIndex = 0; primitiveIndex < targetNode.mesh->primitives_count; ++primitiveIndex)
		{
			if (targetNode.mesh->primitives[primitiveIndex].targets_count != 4u)
			{
				return 0;
			}
		}
		return 4u;
	}

	static std::uint32_t FindSamplerIndex(const cgltf_animation& animation, const cgltf_animation_sampler* sampler) noexcept
	{
		if (sampler == nullptr)
		{
			return (std::numeric_limits<std::uint32_t>::max)();
		}

		for (cgltf_size samplerIndex = 0; samplerIndex < animation.samplers_count; ++samplerIndex)
		{
			if (&animation.samplers[samplerIndex] == sampler)
			{
				return static_cast<std::uint32_t>(samplerIndex);
			}
		}

		return (std::numeric_limits<std::uint32_t>::max)();
	}

	static ImportedSkeletonIndex FindSkeletonForSkin(const SourceImportOutput& output, const cgltf_data& data, const cgltf_skin* skin) noexcept
	{
		if (skin == nullptr)
		{
			return kInvalidImportedSkeletonIndex;
		}
		const cgltf_size sourceSkinIndex = cgltf_skin_index(&data, skin);
		if (sourceSkinIndex >= data.skins_count)
		{
			return kInvalidImportedSkeletonIndex;
		}
		for (std::size_t skeletonIndex = 0; skeletonIndex < output.scene.skeletons.size(); ++skeletonIndex)
		{
			if (output.scene.skeletons[skeletonIndex].sourceSkinIndex == sourceSkinIndex)
			{
				return static_cast<ImportedSkeletonIndex>(skeletonIndex);
			}
		}
		return kInvalidImportedSkeletonIndex;
	}

	static void ImportChannel(
	    const cgltf_data& data,
	    const cgltf_animation& animation,
	    const cgltf_animation_channel& channel,
	    std::size_t channelIndex,
	    const SourceImportOutput& output,
	    std::vector<ImportedAnimationTargetPath>& samplerTargetPaths,
	    ImportedAnimationClip& clip)
	{
		const ImportedAnimationTargetPath targetPath = ToImportedTargetPath(channel.target_path);
		const std::uint32_t samplerIndex = FindSamplerIndex(animation, channel.sampler);
		const bool bInvalidAnimationChannelsamplers = targetPath == ImportedAnimationTargetPath::Unknown || samplerIndex >= clip.samplers.size();
		const bool invalidAnimationChannel = bInvalidAnimationChannelsamplers || (channel.target_node == nullptr);
		
		if (invalidAnimationChannel)
		{
			throw Diagnostics::Error(std::format("glTF animation channel {} has an invalid target or sampler.", channelIndex));
		}
		if (samplerTargetPaths[samplerIndex] != ImportedAnimationTargetPath::Unknown && samplerTargetPaths[samplerIndex] != targetPath)
		{
			throw Diagnostics::Error(std::format("glTF animation channel {} reuses a sampler for another target path.", channelIndex));
		}
		samplerTargetPaths[samplerIndex] = targetPath;

		const std::uint32_t morphWeightCount = targetPath == ImportedAnimationTargetPath::Weights ? ResolveMorphWeightCount(*channel.target_node) : 0u;
		if (targetPath == ImportedAnimationTargetPath::Weights && morphWeightCount != 4u)
		{
			throw Diagnostics::Error(std::format("glTF animation channel {} does not target exactly four morph weights.", channelIndex));
		}
		if (clip.samplers[samplerIndex].keyframes.empty())
		{
			clip.samplers[samplerIndex] = GltfAnimationSampler::Import(*channel.sampler, targetPath, morphWeightCount, clip.durationSeconds);
		}

		const std::uint32_t targetNodeIndex = static_cast<std::uint32_t>(cgltf_node_index(&data, channel.target_node));
		const auto [jointSkeletonIndex, targetJointIndex] = output.scene.FindSkeletonJointForNode(targetNodeIndex);
		const ImportedSkeletonIndex targetSkeletonIndex = targetPath == ImportedAnimationTargetPath::Weights ? FindSkeletonForSkin(output, data, channel.target_node->skin) : jointSkeletonIndex;
		if (targetSkeletonIndex == kInvalidImportedSkeletonIndex || (targetPath != ImportedAnimationTargetPath::Weights && targetJointIndex == (std::numeric_limits<std::uint32_t>::max)()))
		{
			throw Diagnostics::Error(std::format("glTF animation channel {} is not owned by an imported skeleton.", channelIndex));
		}
		if (clip.targetSkeletonIndex != kInvalidImportedSkeletonIndex && clip.targetSkeletonIndex != targetSkeletonIndex)
		{
			throw Diagnostics::Error(std::format("glTF animation channel {} targets another skeleton.", channelIndex));
		}
		clip.targetSkeletonIndex = targetSkeletonIndex;

		const bool duplicateAnimationChannel = std::any_of(
				        clip.channels.begin(),
				        clip.channels.end(),
				        [targetPath, targetNodeIndex](const ImportedAnimationChannel& importedChannel)
				        { return importedChannel.targetPath == targetPath && importedChannel.targetNodeIndex == targetNodeIndex; });
		
		if (duplicateAnimationChannel)
		{
			throw Diagnostics::Error(std::format("glTF animation channel {} duplicates an existing target path.", channelIndex));
		}

		clip.channels.push_back(ImportedAnimationChannel{.targetPath = targetPath, .targetNodeIndex = targetNodeIndex, .targetJointIndex = targetJointIndex, .samplerIndex = samplerIndex});
	}
}

void GltfAnimationImporter::ImportAnimations(const cgltf_data* data, SourceImportOutput& output)
{
	if (data == nullptr)
	{
		throw Diagnostics::Error("glTF animation import has no parsed scene.");
	}
	if (data->animations_count == 0)
	{
		return;
	}
	// Sparkle animation clips are skeleton-owned. Preserve static scene import for
	// glTF assets that contain only node or morph animation until a node-animation
	// runtime exists; skeletal assets continue through strict channel validation.
	if (output.scene.skeletons.empty())
	{
		return;
	}

	output.scene.animations.reserve(data->animations_count);
	for (cgltf_size animationIndex = 0; animationIndex < data->animations_count; ++animationIndex)
	{
		const cgltf_animation& animation = data->animations[animationIndex];
		ImportedAnimationClip clip;
		clip.name = animation.name != nullptr ? animation.name : "";
		clip.sourceAnimationIndex = static_cast<std::uint32_t>(animationIndex);
		clip.samplers.resize(animation.samplers_count);
		clip.channels.reserve(animation.channels_count);
		std::vector<ImportedAnimationTargetPath> samplerTargetPaths(animation.samplers_count, ImportedAnimationTargetPath::Unknown);

		for (cgltf_size channelIndex = 0; channelIndex < animation.channels_count; ++channelIndex)
		{
			ImportChannel(*data, animation, animation.channels[channelIndex], channelIndex, output, samplerTargetPaths, clip);
		}

		if (!clip.IsValid())
		{
			throw Diagnostics::Error(std::format("glTF animation {} has no complete channel set.", animationIndex));
		}
		output.scene.animations.push_back(std::move(clip));
	}
}
