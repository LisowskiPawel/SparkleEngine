#pragma once

namespace ECS
{
	class EntityRegistry;

	class EntityIterationScope final
	{
	public:
		explicit EntityIterationScope(EntityRegistry& registry) noexcept;
		~EntityIterationScope();
		EntityIterationScope(const EntityIterationScope&) = delete;
		EntityIterationScope& operator=(const EntityIterationScope&) = delete;
		EntityIterationScope(EntityIterationScope&&) = delete;
		EntityIterationScope& operator=(EntityIterationScope&&) = delete;

		bool IsValid() const noexcept { return m_registry != nullptr; }

	private:
		EntityRegistry* m_registry = nullptr;
	};
}
