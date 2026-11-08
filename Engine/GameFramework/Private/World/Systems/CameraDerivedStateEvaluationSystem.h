#pragma once

namespace ECS
{
	struct LocalTransform;
	struct CameraDerivedState;

	namespace CameraDerivedStateEvaluationSystem
	{
		void Evaluate(const LocalTransform& local, CameraDerivedState& derived) noexcept;
	}
}
