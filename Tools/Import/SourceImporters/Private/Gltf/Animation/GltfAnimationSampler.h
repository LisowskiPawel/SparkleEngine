#pragma once

#include "Types/ImportedAnimation.h"

struct cgltf_animation_sampler;

namespace GltfAnimationSampler
{
	ImportedAnimationSampler Import(const cgltf_animation_sampler& sampler, ImportedAnimationTargetPath targetPath, std::uint32_t morphWeightCount, float& inOutClipDurationSeconds);
}
