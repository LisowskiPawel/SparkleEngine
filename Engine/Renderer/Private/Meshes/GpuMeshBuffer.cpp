#include "PCH.h"
#include "Meshes/GpuMeshBuffer.h"

#include "Core/Public/Diagnostics/Verify.h"
#include "Core/Public/Strings/StringUtils.h"
#include "RHI/Public/Commands/RenderCommandList.h"
#include "RHI/Public/Descriptors/RhiDescriptorService.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "RHI/Public/Resources/RhiResourceService.h"
#include "RHI/Public/Resources/RhiUploadService.h"

#include <format>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_gpuMeshBufferLogger, "Renderer.GpuMeshBuffer");

GpuMeshBuffer::GpuMeshBuffer() noexcept = default;

GpuMeshBuffer::~GpuMeshBuffer() noexcept
{
	Release();
}

void GpuMeshBuffer::Upload(
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
	if (!m_resource)
	{
		Release();
		Diagnostics::Fatal(g_gpuMeshBufferLogger, __FILE__, __LINE__, std::format("GPU mesh buffer '{}' allocation failed.", Strings::ToNarrow(debugName)));
	}

	const ResourceState finalState = bStructured ? ResourceState::ShaderResource : ResourceState::Common;
	const bool bUploaded = renderHardwareInterface.GetUploadService().UploadBuffer(commandList, m_resource, payload, finalState, uploadDebugName);
	if (!bUploaded)
	{
		Release();
		Diagnostics::Fatal(g_gpuMeshBufferLogger, __FILE__, __LINE__, std::format("GPU mesh buffer '{}' upload failed.", Strings::ToNarrow(debugName)));
	}

	if (bStructured)
	{
		const RhiResourceViewDesc viewDesc = RhiResourceViewDesc::BufferShaderResource(resources.GetResourceHandle(m_resource), payload.size_bytes(), strideInBytes);
		m_view = renderHardwareInterface.GetDescriptorService().CreateResourceView(viewDesc);
		m_shaderResourceView = renderHardwareInterface.GetDescriptorService().GetResourceViewGpuHandle(m_view);
		if (!m_shaderResourceView)
		{
			Release();
			Diagnostics::Fatal(g_gpuMeshBufferLogger, __FILE__, __LINE__, std::format("GPU mesh buffer '{}' has no shader-resource descriptor.", Strings::ToNarrow(debugName)));
		}
	}
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
