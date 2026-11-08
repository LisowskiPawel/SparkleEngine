#pragma once

#include <span>
#include <vector>

namespace Files
{
	struct FilePublication;
}
struct CookedSceneBuild;

void StageCookedSceneManifestsAndRegistry(std::span<const CookedSceneBuild* const> builds, std::vector<Files::FilePublication>& outPublication);
