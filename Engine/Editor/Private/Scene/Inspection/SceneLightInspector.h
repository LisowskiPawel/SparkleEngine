#pragma once

#include <cstdint>
#include "World/EntityId.h"

class EditorTransactionHistory;
struct SceneLightDesc;

namespace SceneLightInspector
{
	void Build(const SceneLightDesc& sceneLight, EntityId lightEntity, EditorTransactionHistory& transactionHistory, std::uint64_t worldGeneration) noexcept;
}
