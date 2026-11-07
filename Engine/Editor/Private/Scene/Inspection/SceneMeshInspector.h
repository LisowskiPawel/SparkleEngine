#pragma once

#include <cstdint>
#include <string>

class EditorTransactionHistory;
struct WorldMeshReadData;

namespace SceneMeshInspector
{
	void Build(const WorldMeshReadData& mesh, EditorTransactionHistory& transactionHistory, std::uint64_t worldGeneration, const std::string& filterText) noexcept;
}
