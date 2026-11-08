#pragma once

#include "GameFramework/Public/World/WorldEditCommand.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ECS
{
	class GameWorldState;
}
class MaterialVariantResourceStore;

// Owns bounded admission, target validation, and world-boundary application of semantic edits.
// GameWorld only establishes the boundary at which this capability runs.
class WorldEditCommandQueue final
{
public:
	WorldEditResult Submit(WorldEditCommand command, std::uint64_t expectedGeneration, std::uint64_t currentGeneration, const ECS::GameWorldState& state, const MaterialVariantResourceStore& variants);
	void Apply(std::uint64_t currentGeneration, ECS::GameWorldState& state, MaterialVariantResourceStore& variants);
	void Clear() noexcept;

private:
	struct PendingEdit final
	{
		std::uint64_t ExpectedGeneration = 0;
		WorldEditCommand Command;
	};

	static bool IsTargetAvailable(const WorldEditPayload& payload, const ECS::GameWorldState& state, const MaterialVariantResourceStore& variants);
	static void ApplyPayload(WorldEditPayload& payload, ECS::GameWorldState& state, MaterialVariantResourceStore& variants);

	static constexpr std::size_t MaximumPendingEditCount = 4096;
	std::vector<PendingEdit> m_pendingEdits;
};
