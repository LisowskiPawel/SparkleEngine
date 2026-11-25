#pragma once

#include <filesystem>

struct CookedSceneManifestBuildData;

namespace CookedSceneManifestWriter
{
	void Write(const CookedSceneManifestBuildData& manifest, const std::filesystem::path& outputPath);
}
