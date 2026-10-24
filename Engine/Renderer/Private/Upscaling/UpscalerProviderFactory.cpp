#include "../PCH.h"
#include "Upscaling/UpscalerProviderFactory.h"

#include "Upscaling/NvidiaDlss/NvidiaDlssUpscalerProvider.h"
#include "Upscaling/UpscalerProvider.h"
#include "Upscaling/UpscalerSettings.h"
#include "Core/Public/Diagnostics/Error.h"

std::unique_ptr<IUpscalerProvider> CreateConfiguredUpscalerProvider()
{
	switch (CVarUpscalerProvider.Get())
	{
		case EUpscalerProviderKind::NvidiaDlss:
			return std::make_unique<NvidiaDlssUpscalerProvider>();

		case EUpscalerProviderKind::Linear:
			return {};

		default:
			throw Diagnostics::Error("Invalid r.Upscaler.Provider. Use 0 (Linear) or 1 (NVIDIA DLSS) in the shared rendering settings.");
	}
}
