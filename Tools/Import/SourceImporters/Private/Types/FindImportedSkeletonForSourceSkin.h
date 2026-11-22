#pragma once

#include "Types/ImportedSkin.h"

struct ImportedScene;

ImportedSkeletonIndex FindImportedSkeletonForSourceSkin(const ImportedScene& scene, std::uint32_t sourceSkinIndex) noexcept;
