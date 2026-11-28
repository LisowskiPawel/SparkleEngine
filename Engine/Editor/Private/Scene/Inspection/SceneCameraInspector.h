#pragma once

#include <cstdint>

class EditorTransactionHistory;
struct WorldCameraReadData;

namespace SceneCameraInspector
{
	void Build(const WorldCameraReadData& camera, EditorTransactionHistory& transactionHistory, std::uint64_t worldGeneration) noexcept;
}
