#pragma once

#include "Diagnostics/AssetCookerDiagnostics.h"
#include "Planning/ProjectCookPlan.h"
#include "ToolWorkProgressEvent.h"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <vector>

class AssetCookerSceneBatch final
{
public:
	static bool Execute(const std::vector<AssetCookerSceneEntry>& sceneEntries, AssetCookerDiagnostics& diagnostics, const ToolWorkProgressCallback& progress = {});

private:
	struct Item;

	static std::uint32_t ResolveWorkerCount() noexcept;
	struct BuildRun;
	static void BuildRange(std::uint32_t begin, std::uint32_t end, std::uint32_t thread, void* argument) noexcept;
	static std::exception_ptr BuildProducts(const std::vector<AssetCookerSceneEntry>& sceneEntries, std::vector<Item>& items, const ToolWorkProgressCallback& progress, std::size_t totalWork);
	static void MergeDiagnostics(std::vector<Item>& items, AssetCookerDiagnostics& diagnostics);
	static bool PublishProducts(std::vector<Item>& items, AssetCookerDiagnostics& diagnostics);
};
