#pragma once

#include "GameFramework/Public/World/WorldReadView.h"

#include <cstdint>
#include <optional>
#include <vector>

struct WorldReadView::Storage final
{
	WorldSequence Sequence = 0;
	std::vector<WorldCameraReadData> Cameras;
	std::vector<WorldLightReadData> Lights;
	std::vector<WorldMeshReadData> Meshes;
	std::optional<SkyEnvironment> Sky;
};
