#include "PCH.h"
#include "Meshes/GpuMeshBuffer.h"

#include "RHI/Public/Commands/RenderCommandList.h"
#include "RHI/Public/Descriptors/RhiDescriptorService.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "RHI/Public/Resources/RhiResourceService.h"
#include "RHI/Public/Resources/RhiUploadService.h"

GpuMeshBuffer::GpuMeshBuffer() noexcept = default;

GpuMeshBuffer::~GpuMeshBuffer() noexcept
{
	Release();
}

bool GpuMeshBuffer::Upload(
    RenderHardwareInterface& renderHardwareInterface,
    RenderCommandList& commandList,
    std::span<const std::byte> payload,
    RhiBufferKind kind,
    std::uint32_t strideInBytes,
    std::wstring_view debugName,
    std::wstring_view uploadDebugName)
{
	Release();
	m_renderHardwareInterface = &renderHardwareInterface;
	const bool bStructured = kind == RhiBufferKind::Structured;
	const RhiBufferResourceDesc desc{.SizeInBytes = payload.size_bytes(), .StrideInBytes = strideInBytes, .Kind = kind, .bAllowRayTracingBuildInput = !bStructured};

	RhiResourceService& resources = renderHardwareInterface.GetResourceService();
	m_resource = resources.CreateBufferResource(desc, ResourceState::CopyDest, RhiMemoryCategory::Mesh, RhiMemoryResidencyClass::DeviceLocal, debugName);
	const ResourceState finalState = bStructured ? ResourceState::ShaderResource : ResourceState::Common;
	const bool bUploaded = m_resource && renderHardwareInterface.GetUploadService().UploadBuffer(commandList, m_resource, payload, finalState, uploadDebugName);
	if (!bUploaded)
	{
		Release();
		return false;
	}
	if (bStructured)
	{
		const RhiResourceViewDesc viewDesc = RhiResourceViewDesc::BufferShaderResource(resources.GetResourceHandle(m_resource), payload.size_bytes(), strideInBytes);
		m_view = renderHardwareInterface.GetDescriptorService().CreateResourceView(viewDesc);
		m_shaderResourceView = renderHardwareInterface.GetDescriptorService().GetResourceViewGpuHandle(m_view);
		if (!m_shaderResourceView)
		{
			Release();
			return false;
		}
	}
	return true;
}

void GpuMeshBuffer::Release() noexcept
{
	if (m_renderHardwareInterface == nullptr)
	{
		return;
	}
	if (m_view)
	{
		m_renderHardwareInterface->GetDescriptorService().ReleaseResourceView(m_view);
		m_view = {};
	}
	if (m_resource)
	{
		m_renderHardwareInterface->GetResourceService().ReleaseOwnedResource(m_resource);
		m_resource = {};
	}
	m_shaderResourceView = {};
	m_renderHardwareInterface = nullptr;
}
