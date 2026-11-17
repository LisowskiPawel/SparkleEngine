#include "PCH.h"

#include "Core/RhiCapabilities.h"

const RhiFormatSupport* RhiCapabilities::FindFormatSupport(PixelFormat format) const noexcept
{
	for (const RhiFormatSupport& support : FormatSupport)
	{
		if (support.Format == format)
		{
			return &support;
		}
	}

	return nullptr;
}

const char* RhiDescriptorModelToString(ERhiDescriptorModel model) noexcept
{
	switch (model)
	{
		case ERhiDescriptorModel::DescriptorTables:
			return "DescriptorTables";
		case ERhiDescriptorModel::DescriptorSets:
			return "DescriptorSets";
		case ERhiDescriptorModel::Unknown:
		default:
			return "Unknown";
	}
}

const char* RhiMemoryAllocatorBackendToString(ERhiMemoryAllocatorBackend backend) noexcept
{
	switch (backend)
	{
		case ERhiMemoryAllocatorBackend::D3D12Managed:
			return "D3D12Managed";
		case ERhiMemoryAllocatorBackend::VulkanManaged:
			return "VulkanManaged";
		case ERhiMemoryAllocatorBackend::Unknown:
		default:
			return "Unknown";
	}
}

const char* RhiExternalFeatureBridgeKindToString(ERhiExternalFeatureBridgeKind kind) noexcept
{
	switch (kind)
	{
		case ERhiExternalFeatureBridgeKind::Interposer:
			return "Interposer";
		case ERhiExternalFeatureBridgeKind::ManualIntegration:
			return "ManualIntegration";
		case ERhiExternalFeatureBridgeKind::None:
		default:
			return "None";
	}
}

const char* RhiBackendVersionSemanticToString(ERhiBackendVersionSemantic semantic) noexcept
{
	switch (semantic)
	{
		case ERhiBackendVersionSemantic::ApiVersion:
			return "ApiVersion";
		case ERhiBackendVersionSemantic::FeatureLevel:
			return "FeatureLevel";
		case ERhiBackendVersionSemantic::Unknown:
		default:
			return "Unknown";
	}
}
