#include "PCH.h"

#include "WorldEditCommandQueue.h"

#include "World/GameWorldState.h"
#include "World/Resources/MaterialVariantResourceStore.h"

#include <type_traits>
#include <utility>

WorldEditResult WorldEditCommandQueue::Submit(
    WorldEditCommand command,
    std::uint64_t expectedGeneration,
    std::uint64_t currentGeneration,
    const ECS::GameWorldState& state,
    const MaterialVariantResourceStore& variants)
{
	WorldEditResult result{.RequestId = command.RequestId};
	if (expectedGeneration != currentGeneration)
	{
		result.Status = WorldEditResultStatus::Stale;
		result.Message = "The editor model belongs to an older world generation.";
		return result;
	}
	if (m_pendingEdits.size() >= MaximumPendingEditCount)
	{
		result.Status = WorldEditResultStatus::Rejected;
		result.Message = "The bounded world-edit queue is full.";
		return result;
	}
	if (!IsTargetAvailable(command.Payload, state, variants))
	{
		result.Status = WorldEditResultStatus::Rejected;
		result.Message = "The semantic edit target or value is unavailable.";
		return result;
	}

	m_pendingEdits.push_back({expectedGeneration, std::move(command)});
	result.Status = WorldEditResultStatus::Accepted;
	return result;
}

void WorldEditCommandQueue::Apply(std::uint64_t currentGeneration, ECS::GameWorldState& state, MaterialVariantResourceStore& variants)
{
	for (PendingEdit& pending : m_pendingEdits)
	{
		if (pending.ExpectedGeneration == currentGeneration)
		{
			ApplyPayload(pending.Command.Payload, state, variants);
		}
	}
	Clear();
}

void WorldEditCommandQueue::Clear() noexcept
{
	m_pendingEdits.clear();
}

bool WorldEditCommandQueue::IsTargetAvailable(const WorldEditPayload& payload, const ECS::GameWorldState& state, const MaterialVariantResourceStore& variants)
{
	return std::visit(
	    [&state, &variants](const auto& command)
	    {
		    using T = std::decay_t<decltype(command)>;
		    if constexpr (std::is_same_v<T, SetActiveCameraCommand> || std::is_same_v<T, SetCameraDescriptionCommand>)
		    {
			    return state.IsCamera(command.Entity);
		    }
		    else if constexpr (std::is_same_v<T, SetLocalTransformCommand> || std::is_same_v<T, SetEntityVisibilityCommand>)
		    {
			    return state.IsAlive(command.Entity);
		    }
		    else if constexpr (std::is_same_v<T, SetLightDescriptionCommand>)
		    {
			    return state.ReadLight(command.Entity).has_value();
		    }
		    else if constexpr (std::is_same_v<T, SetMaterialVariantCommand>)
		    {
			    return command.Value < variants.GetCount();
		    }
		    else
		    {
			    return true;
		    }
	    },
	    payload);
}

void WorldEditCommandQueue::ApplyPayload(WorldEditPayload& payload, ECS::GameWorldState& state, MaterialVariantResourceStore& variants)
{
	std::visit(
	    [&state, &variants](auto& command)
	    {
		    using T = std::decay_t<decltype(command)>;
		    if constexpr (std::is_same_v<T, SetActiveCameraCommand>)
		    {
			    (void) state.SetActiveCamera(command.Entity);
		    }
		    else if constexpr (std::is_same_v<T, SetLocalTransformCommand>)
		    {
			    (void) state.WriteTransform(command.Entity, command.Value);
		    }
		    else if constexpr (std::is_same_v<T, SetCameraDescriptionCommand>)
		    {
			    (void) state.WriteCameraDesc(command.Entity, command.Value);
		    }
		    else if constexpr (std::is_same_v<T, SetEntityVisibilityCommand>)
		    {
			    (void) state.WriteVisibility(command.Entity, command.bValue);
		    }
		    else if constexpr (std::is_same_v<T, SetLightDescriptionCommand>)
		    {
			    (void) state.WriteLight(command.Entity, std::move(command.Value));
		    }
		    else if constexpr (std::is_same_v<T, SetSkyEnvironmentCommand>)
		    {
			    if (command.Value)
			    {
				    state.WriteSkyEnvironment(std::move(*command.Value));
			    }
			    else
			    {
				    state.RemoveSkyEnvironment();
			    }
		    }
		    else if constexpr (std::is_same_v<T, SetMaterialVariantCommand>)
		    {
			    (void) variants.Apply(command.Value, [&state](EntityId entity, MaterialHandle material) { return state.WriteMeshMaterial(entity, material); });
		    }
	    },
	    payload);
}
