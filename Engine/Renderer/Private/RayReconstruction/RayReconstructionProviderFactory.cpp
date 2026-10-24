#include "../PCH.h"
#include "RayReconstruction/RayReconstructionProviderFactory.h"

#include "RayReconstruction/NvidiaDlssRayReconstruction/NvidiaDlssRayReconstructionProvider.h"
#include "RayReconstruction/RayReconstructionProvider.h"
#include "RayReconstruction/RayReconstructionSettings.h"
#include "Core/Public/Diagnostics/Error.h"

std::unique_ptr<IRayReconstructionProvider> CreateConfiguredRayReconstructionProvider()
{
	switch (CVarRayReconstructionMode.Get())
	{
		case EngineRayReconstructionMode::NvidiaDlssRayReconstruction:
			return std::make_unique<NvidiaDlssRayReconstructionProvider>();

		case EngineRayReconstructionMode::Off:
			return {};

		default:
			throw Diagnostics::Error("Invalid r.RayReconstruction.Mode. Use 0 (Off) or 1 (NVIDIA DLSS Ray Reconstruction) in the shared rendering settings.");
	}
}
