#pragma once

#include "World/ECS/ComponentStorage.h"
#include "World/ECS/Components/AnimationComponents.h"
#include "World/ECS/Components/EditorComponents.h"
#include "World/ECS/Components/RenderingComponents.h"
#include "World/ECS/Components/TransformComponents.h"

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ECS
{
	using WorldComponentStores = std::tuple<
	    ComponentStorage<LocalTransform>,
	    ComponentStorage<WorldTransform>,
	    ComponentStorage<CameraDerivedState>,
	    ComponentStorage<MeshInstance>,
	    ComponentStorage<Visibility>,
	    ComponentStorage<Camera>,
	    ComponentStorage<Light>,
	    ComponentStorage<AnimationState>,
	    ComponentStorage<MorphState>,
	    ComponentStorage<SkinningState>,
	    ComponentStorage<Name>,
	    ComponentStorage<AuthoredIdentity>,
	    ComponentStorage<EditorMetadata>>;

	// Access identity is local to the fixed world schema, never allocated or persisted.
	template <ComponentStorageCompatible T> consteval std::uint32_t GetWorldComponentIndex()
	{
		return []<std::size_t... Indices>(std::index_sequence<Indices...>)
		{
			static_assert((std::is_same_v<ComponentStorage<T>, std::tuple_element_t<Indices, WorldComponentStores>> || ...), "Component must belong to the fixed world schema.");
			return static_cast<std::uint32_t>(((std::is_same_v<ComponentStorage<T>, std::tuple_element_t<Indices, WorldComponentStores>> ? Indices : 0) + ...));
		}(std::make_index_sequence<std::tuple_size_v<WorldComponentStores>>{});
	}
}
