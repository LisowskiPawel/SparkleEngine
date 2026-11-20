#include "PCH.h"
#include "World/Systems/CameraDerivedStateEvaluationSystem.h"

#include "Core/Public/Math/WorldCoordinateSystem.h"
#include "World/ECS/Components/TransformComponents.h"

#include <cmath>

namespace ECS
{
	void CameraDerivedStateEvaluationSystem::Evaluate(const LocalTransform& local, CameraDerivedState& derived) noexcept
	{
		const DirectX::XMVECTOR forwardAxis = DirectX::XMVectorSet(WorldCoordinates::kForwardX, WorldCoordinates::kForwardY, WorldCoordinates::kForwardZ, 0.0f);
		const DirectX::XMVECTOR rotation = DirectX::XMLoadFloat4(&local.Rotation);
		const DirectX::XMVECTOR forward = DirectX::XMVector3Rotate(forwardAxis, rotation);
		const DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(forward);
		DirectX::XMStoreFloat3(&derived.Direction, direction);
	}
}
