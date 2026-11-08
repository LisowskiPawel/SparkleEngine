#pragma once

namespace ECS
{
	struct LocalTransform;
	struct WorldTransform;

	namespace TransformEvaluationSystem
	{
		void Evaluate(const LocalTransform& local, WorldTransform& world) noexcept;
	}
}
