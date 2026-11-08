#pragma once

#include "SourceImportOutput.h"

struct cgltf_data;

namespace GltfCameraImporter
{
	void ImportCameras(const cgltf_data* data, SourceImportOutput& output);
}
