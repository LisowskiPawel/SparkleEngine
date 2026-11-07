#pragma once

#include <cstdint>
#include <string>

class EditorTransactionHistory;
struct WorldCameraReadData;

namespace SceneCameraInspector
{
	void Build(const WorldCameraReadData& camera, EditorTransactionHistory& transactionHistory, std::uint64_t worldGeneration, const std::string& filterText) noexcept;
}
