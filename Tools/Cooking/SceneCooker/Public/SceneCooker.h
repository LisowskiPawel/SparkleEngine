#pragma once

#include <filesystem>

struct CookedSceneBuild;
struct CookedSceneIdentity;
struct SourceImportOutput;

namespace SceneCooker
{
	CookedSceneIdentity ResolveSceneIdentity(const std::filesystem::path& sourceScenePath);
	void BuildManifest(const SourceImportOutput& importOutput, CookedSceneBuild& outBuild);
}
