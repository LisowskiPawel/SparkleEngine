#include "../PCH.h"
#include "Resources/OwnedStructuredBuffer.h"

#include "RHI/Public/Resources/RhiResourceService.h"

#include <utility>

OwnedStructuredBuffer::~OwnedStructuredBuffer() noexcept
{
	Reset();
}

OwnedStructuredBuffer::OwnedStructuredBuffer(OwnedStructuredBuffer&& other) noexcept
{
	*this = std::move(other);
}

OwnedStructuredBuffer& OwnedStructuredBuffer::operator=(OwnedStructuredBuffer&& other) noexcept
{
	if (this == &other)
	{
		return *this;
	}

	Reset();
	m_resourceService = other.m_resourceService;
	m_resource = other.m_resource;
	other.m_resourceService = nullptr;
	other.m_resource = {};
	return *this;
}

OwnedStructuredBuffer OwnedStructuredBuffer::Upload(RhiResourceService& resourceService, const void* data, std::size_t sizeInBytes, std::uint32_t strideInBytes, std::wstring_view debugName)
{
	OwnedStructuredBuffer buffer;
	buffer.m_resourceService = &resourceService;
	const RhiBufferResourceDesc desc{.SizeInBytes = sizeInBytes, .StrideInBytes = strideInBytes, .Kind = RhiBufferKind::Structured};
	buffer.m_resource = resourceService.CreateHostUploadBuffer(desc, data, RhiMemoryCategory::Mesh, debugName.empty() ? L"StructuredBuffer" : debugName);
	if (!buffer.m_resource)
	{
		return {};
	}

	return buffer;
}

bool OwnedStructuredBuffer::Write(std::size_t destinationOffsetInBytes, const void* data, std::size_t sizeInBytes) noexcept
{
	return m_resourceService != nullptr && m_resourceService->WriteBufferResource(m_resource, destinationOffsetInBytes, data, sizeInBytes);
}

void OwnedStructuredBuffer::Reset() noexcept
{
	if (m_resourceService != nullptr && m_resource)
	{
		m_resourceService->ReleaseOwnedResource(m_resource);
	}
	m_resourceService = nullptr;
	m_resource = {};
}
