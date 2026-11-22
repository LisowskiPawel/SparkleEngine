#include "PCH.h"
#include "GameFramework/Public/Assets/Cooked/CookedSceneCameraRecord.h"

#include "CookedSceneCameraBuilder.h"

#include "Core/Public/Diagnostics/Error.h"

#include <cstring>
#include <format>

namespace CookedSceneCameraBuilder
{
	void BuildCameras(const SourceImportOutput& importOutput, CookedSceneBuild& outBuild)
	{
		outBuild.manifest.cameras.clear();
		outBuild.manifest.cameras.reserve(importOutput.scene.cameras.size());

		for (std::size_t cameraIndex = 0; cameraIndex < importOutput.scene.cameras.size(); ++cameraIndex)
		{
			const ImportedCamera& importedCamera = importOutput.scene.cameras[cameraIndex];
			const bool bInvalidCameraDescription = importedCamera.name.size() >= Assets::kCookedSceneCameraNameCapacity || importedCamera.projectionKind != ImportedCameraProjectionKind::Perspective;

			if (bInvalidCameraDescription)
			{
				throw Diagnostics::Error(std::format("Imported camera {} exceeds the cooked camera contract.", cameraIndex));
			}
			Assets::CookedSceneCameraRecord cameraRecord;
			if (!importedCamera.name.empty())
			{
				std::memcpy(cameraRecord.name, importedCamera.name.data(), importedCamera.name.size());
			}
			cameraRecord.worldTransform = importedCamera.worldTransform;
			cameraRecord.projectionKind = Assets::CookedSceneCameraProjectionKind::Perspective;
			cameraRecord.fovYRadians = importedCamera.fovYRadians;
			cameraRecord.nearZ = importedCamera.nearZ;
			cameraRecord.farZ = importedCamera.farZ;
			cameraRecord.sourceNodeIndex = importedCamera.sourceNodeIndex;
			outBuild.manifest.cameras.push_back(cameraRecord);
		}
	}
}
