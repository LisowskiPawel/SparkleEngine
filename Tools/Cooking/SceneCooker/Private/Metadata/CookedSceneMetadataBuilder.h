#pragma once

#include <cstdint>

struct SourceImportOutput;

namespace CookedSceneMetadataBuilder
{
	std::uint32_t BuildFeatureFlags(const SourceImportOutput& importOutput) noexcept;
}
