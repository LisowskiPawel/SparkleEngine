#pragma once

#include <cstdint>
#include <filesystem>
#include <string_view>

namespace SparkleLauncher
{
	enum class CookedShaderPublicationState : std::uint8_t
	{
		Missing,
		Invalid,
		Ready
	};

	struct CookedContentReadiness
	{
		bool bMeshesReady = false;
		bool bTexturesReady = false;
		CookedShaderPublicationState Shaders = CookedShaderPublicationState::Missing;
	};

	CookedContentReadiness InspectCookedContentReadiness(const std::filesystem::path& workspaceRoot, std::string_view projectId, std::string_view variant = {});
}
