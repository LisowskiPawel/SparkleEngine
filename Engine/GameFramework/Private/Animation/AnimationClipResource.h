#pragma once

#include "GameFramework/Public/Assets/Cooked/CookedAssetCommon.h"

#include <ozz/animation/runtime/animation.h>
#include <ozz/animation/runtime/track.h>

#include <cstdint>
#include <vector>

struct AnimationMorphTrack final
{
	std::uint32_t targetNodeIndex = 0;
	ozz::animation::Float4Track track;
};

struct AnimationClipResource final
{
	Assets::CookedAssetId animationAssetId = Assets::InvalidCookedAssetId;
	Assets::CookedAssetId targetSkeletonAssetId = Assets::InvalidCookedAssetId;
	std::uint32_t sourceAnimationIndex = 0;
	float durationSeconds = 0.0f;
	ozz::animation::Animation animation;
	std::vector<AnimationMorphTrack> morphTracks;
};
