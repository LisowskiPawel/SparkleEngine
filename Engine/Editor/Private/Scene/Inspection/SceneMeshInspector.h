#pragma once

#include <cstdint>

class EditorTransactionHistory;
struct WorldMeshReadData;

namespace SceneMeshInspector
{
	void Build(const WorldMeshReadData& mesh, EditorTransactionHistory& transactionHistory, std::uint64_t worldGeneration) noexcept;
}
