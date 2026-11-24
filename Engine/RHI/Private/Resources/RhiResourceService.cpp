#include "Resources/RhiResourceService.h"

RhiOwnedResourceHandle RhiResourceService::CreateHostUploadBuffer(const RhiBufferResourceDesc& desc, const void* data, RhiMemoryCategory category, std::wstring_view debugName)
{
	const bool bRequiresStride = desc.Kind == RhiBufferKind::Vertex || desc.Kind == RhiBufferKind::Structured;
	const bool bInvalidStride = bRequiresStride && desc.StrideInBytes == 0;
	if (data == nullptr || desc.SizeInBytes == 0 || bInvalidStride)
	{
		return {};
	}

	const RhiOwnedResourceHandle resource = CreateBufferResource(desc, ResourceState::Common, category, RhiMemoryResidencyClass::HostUpload, debugName);
	if (!resource)
	{
		return {};
	}
	if (!WriteBufferResource(resource, 0, data, static_cast<std::size_t>(desc.SizeInBytes)))
	{
		ReleaseOwnedResource(resource);
		return {};
	}

	return resource;
}
