#pragma once

#include "World/ECS/ComponentStorage.h"
#include "World/ECS/Components/AnimationComponents.h"
#include "World/ECS/Components/EditorComponents.h"
#include "World/ECS/Components/RenderingComponents.h"
#include "World/ECS/Components/TransformComponents.h"

#include <tuple>

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
	    ComponentStorage<AuthoredIdentity>>;
}
