#pragma once

#include "SourceImportOutput.h"

struct cgltf_data;

namespace GltfAnimationImporter
{
	void ImportAnimations(const cgltf_data* data, SourceImportOutput& output);
}
