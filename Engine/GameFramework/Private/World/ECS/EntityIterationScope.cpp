#include "PCH.h"
#include "World/ECS/EntityIterationScope.h"

#include "World/ECS/EntityRegistry.h"

namespace ECS
{
	EntityIterationScope::EntityIterationScope(EntityRegistry& registry) noexcept
	{
		if (!registry.m_bStructureFrozen)
		{
			registry.m_bStructureFrozen = true;
			m_registry = &registry;
		}
	}

	EntityIterationScope::~EntityIterationScope()
	{
		if (m_registry != nullptr)
		{
			m_registry->m_bStructureFrozen = false;
		}
	}
}
