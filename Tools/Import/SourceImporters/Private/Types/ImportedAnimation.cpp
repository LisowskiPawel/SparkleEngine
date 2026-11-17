#include "PCH.h"

#include "Types/ImportedAnimation.h"

bool ImportedAnimationSampler::IsValid() const noexcept
{
	return !keyframes.empty();
}

bool ImportedAnimationClip::IsValid() const noexcept
{
	const bool bHasChannels = !samplers.empty() && !channels.empty();
	return bHasChannels && durationSeconds >= 0.0f;
}
