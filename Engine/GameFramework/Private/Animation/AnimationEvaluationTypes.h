#pragma once

#include "Animation/SkeletonResource.h"

namespace ECS
{
	struct SkeletonEvaluationData final
	{
		const SkeletonResource* Resource = nullptr;

		bool IsValid() const noexcept { return Resource != nullptr; }
	};
}
