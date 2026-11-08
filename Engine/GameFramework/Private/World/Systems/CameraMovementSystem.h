#pragma once

#include "GameFramework/Public/World/EntityId.h"
#include "World/Systems/CameraSimulationInput.h"

namespace ECS
{
	struct Camera;
	struct LocalTransform;
	class EntityRegistry;

	class CameraMovementSystem final
	{
	public:
		CameraMovementSystem(EntityRegistry& registry, EntityId activeCamera, const CameraSimulationInput& input) noexcept;

		EntityId Update();

	private:
		bool Apply(Camera& camera, LocalTransform& transform) const noexcept;

		EntityRegistry& m_registry;
		EntityId m_activeCamera;
		const CameraInputIntent& m_intent;
		const CameraNavigationSettings& m_navigationSettings;
		float m_deltaSeconds = 0.0f;
	};
}
