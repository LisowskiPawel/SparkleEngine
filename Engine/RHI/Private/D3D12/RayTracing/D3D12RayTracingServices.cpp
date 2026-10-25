#include "PCH.h"

#include "D3D12/RayTracing/D3D12RayTracingServices.h"
#include "D3D12/RayTracing/D3D12RayTracingShaderTable.h"

#include "Core/Public/Diagnostics/Error.h"
#include "D3D12/D3D12TypeConversions.h"
#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/Memory/D3D12GpuAllocation.h"
#include "D3D12/Memory/D3D12GpuMemoryAllocator.h"
#include "D3D12/Resources/D3D12ResourceService.h"
#include "Validation/RhiContract.h"

#include <cstring>
#include <d3d12.h>
#include <memory>
#include <string>

namespace D3D12RayTracingText
{
	std::wstring MakeDebugName(std::wstring_view debugName, std::wstring_view defaultDebugName)
	{
		return debugName.empty() ? std::wstring(defaultDebugName) : std::wstring(debugName);
	}
}

D3D12RayTracingServices::D3D12RayTracingServices(D3D12Rhi& rhi, D3D12GpuMemoryAllocator& memoryAllocator, D3D12ResourceService& resourceService, D3D12NvapiRayTracingProvider& nvapiProvider) noexcept :
    m_rhi(&rhi),
    m_memoryAllocator(&memoryAllocator),
    m_resourceService(&resourceService),
    m_nvapiProvider(&nvapiProvider),
    m_tlasServices(rhi, memoryAllocator),
    m_ptlasServices(rhi, memoryAllocator, nvapiProvider)
{
}

RhiTlasService& D3D12RayTracingServices::GetTlasService() noexcept
{
	return m_tlasServices;
}

const RhiTlasService& D3D12RayTracingServices::GetTlasService() const noexcept
{
	return m_tlasServices;
}

RhiPtlasService& D3D12RayTracingServices::GetPtlasService() noexcept
{
	return m_ptlasServices;
}

const RhiPtlasService& D3D12RayTracingServices::GetPtlasService() const noexcept
{
	return m_ptlasServices;
}

RhiRayTracingCapabilities D3D12RayTracingServices::GetCapabilities() const noexcept
{
	return m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities() : RhiRayTracingCapabilities{};
}

RhiRayTracingAccelerationStructurePrebuildInfo D3D12RayTracingServices::GetBottomLevelAccelerationStructurePrebuildInfo(const RhiRayTracingGeometryDesc& geometry) const noexcept
{
	if (m_rhi == nullptr || !m_rhi->GetRayTracingCapabilities().SupportsAccelerationStructure || !RhiContract::IsRayTracingGeometryDescUsable(geometry))
	{
		return {};
	}

	D3D12_RAYTRACING_GEOMETRY_DESC nativeGeometry{};
	nativeGeometry.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	nativeGeometry.Flags = geometry.Opaque ? D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE : D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
	nativeGeometry.Triangles.Transform3x4 = 0;
	nativeGeometry.Triangles.IndexFormat = D3D12TypeConversions::ToIndexFormat(geometry.IndexFormat);
	nativeGeometry.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	nativeGeometry.Triangles.IndexCount = geometry.IndexCount;
	nativeGeometry.Triangles.VertexCount = geometry.VertexCount;
	nativeGeometry.Triangles.IndexBuffer = 0;
	nativeGeometry.Triangles.VertexBuffer.StartAddress = 0;
	nativeGeometry.Triangles.VertexBuffer.StrideInBytes = geometry.VertexStrideInBytes;

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{};
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &nativeGeometry;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO nativeInfo{};
	m_rhi->GetDevice()->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &nativeInfo);

	RhiRayTracingAccelerationStructurePrebuildInfo prebuildInfo{
	    .ResultDataMaxSizeInBytes = nativeInfo.ResultDataMaxSizeInBytes,
	    .ScratchDataSizeInBytes = nativeInfo.ScratchDataSizeInBytes,
	    .UpdateScratchDataSizeInBytes = nativeInfo.UpdateScratchDataSizeInBytes};

	return prebuildInfo;
}

RhiRayTracingAccelerationStructurePrebuildInfo D3D12RayTracingServices::GetTlasPrebuildInfo(std::uint32_t instanceCount, ERhiTlasBuildFlags buildFlags) const noexcept
{
	return m_tlasServices.GetTlasPrebuildInfo(instanceCount, buildFlags);
}

RhiPtlasBuildSizes D3D12RayTracingServices::GetPtlasBuildSizes(const RhiPtlasDesc& desc) const noexcept
{
	return m_ptlasServices.GetPtlasBuildSizes(desc);
}

RhiOwnedResourceHandle D3D12RayTracingServices::CreatePtlasBuffer(const RhiPtlasBuildSizes& sizes, std::wstring_view debugName)
{
	return m_ptlasServices.CreatePtlasBuffer(sizes, debugName);
}

RhiOwnedResourceHandle D3D12RayTracingServices::CreatePtlasOperationBuffer(const RhiPtlasOperationPackDesc& operationPack, std::wstring_view debugName)
{
	return m_ptlasServices.CreatePtlasOperationBuffer(operationPack, debugName);
}

RhiOwnedResourceHandle D3D12RayTracingServices::CreateScratchBuffer(std::uint64_t sizeInBytes, std::wstring_view debugName)
{
	const std::uint64_t scratchAlignment = m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities().ScratchBufferByteAlignment : 0;
	if (m_rhi == nullptr || m_memoryAllocator == nullptr || !RhiContract::IsRayTracingBufferSizeUsable(sizeInBytes, scratchAlignment))
	{
		return {};
	}

	const D3D12_RESOURCE_DESC resourceDesc = D3D12TypeConversions::BuildBufferResourceDesc(RhiBufferResourceDesc{.SizeInBytes = sizeInBytes, .AllowUnorderedAccess = true});

	std::unique_ptr<D3D12GpuAllocationRecord> ownedRecord = m_memoryAllocator->CreateBuffer(
	    resourceDesc,
	    D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
	    RhiMemoryCategory::RayTracing,
	    RhiMemoryResidencyClass::DeviceLocal,
	    D3D12RayTracingText::MakeDebugName(debugName, L"RayTracingScratch"));

	return ownedRecord != nullptr ? MakeD3D12OwnedResourceHandle(std::move(ownedRecord)) : RhiOwnedResourceHandle{};
}

RhiOwnedResourceHandle D3D12RayTracingServices::CreateRayTracingScratchBuffer(std::uint64_t sizeInBytes, std::wstring_view debugName)
{
	return CreateScratchBuffer(sizeInBytes, debugName);
}

RhiOwnedResourceHandle D3D12RayTracingServices::CreateAccelerationStructureBuffer(std::uint64_t sizeInBytes, ERhiRayTracingAccelerationStructureType, std::wstring_view debugName)
{
	const std::uint64_t asAlignment = m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities().AccelerationStructureByteAlignment : 0;
	if (m_rhi == nullptr || m_memoryAllocator == nullptr || !RhiContract::IsRayTracingBufferSizeUsable(sizeInBytes, asAlignment))
	{
		return {};
	}

	const D3D12_RESOURCE_DESC resourceDesc = D3D12TypeConversions::BuildBufferResourceDesc(RhiBufferResourceDesc{.SizeInBytes = sizeInBytes, .AllowUnorderedAccess = true});

	std::unique_ptr<D3D12GpuAllocationRecord> ownedRecord = m_memoryAllocator->CreateBuffer(
	    resourceDesc,
	    D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
	    RhiMemoryCategory::RayTracing,
	    RhiMemoryResidencyClass::DeviceLocal,
	    D3D12RayTracingText::MakeDebugName(debugName, L"RayTracingAccelerationStructure"));

	return ownedRecord != nullptr ? MakeD3D12OwnedResourceHandle(std::move(ownedRecord)) : RhiOwnedResourceHandle{};
}

RhiOwnedResourceHandle D3D12RayTracingServices::CreateRayTracingAccelerationStructureBuffer(std::uint64_t sizeInBytes, ERhiRayTracingAccelerationStructureType type, std::wstring_view debugName)
{
	return CreateAccelerationStructureBuffer(sizeInBytes, type, debugName);
}

RhiOwnedResourceHandle D3D12RayTracingServices::CreateInstanceBuffer(const RhiRayTracingInstanceDesc* instances, std::uint32_t instanceCount, std::wstring_view debugName)
{
	return m_tlasServices.CreateTlasInstanceBuffer(instances, instanceCount, debugName);
}

RhiOwnedResourceHandle D3D12RayTracingServices::CreateRayTracingInstanceBuffer(const RhiRayTracingInstanceDesc* instances, std::uint32_t instanceCount, std::wstring_view debugName)
{
	return CreateInstanceBuffer(instances, instanceCount, debugName);
}

bool D3D12RayTracingServices::BuildPtlas(ID3D12GraphicsCommandList7* commandList, const RhiPtlasBuildCommandDesc& desc) const noexcept
{
	const RhiRayTracingCapabilities capabilities = GetCapabilities();
	if (!capabilities.Groups.Ptlas.Supported)
	{
		return false;
	}
	return m_nvapiProvider != nullptr && m_nvapiProvider->BuildPtlas(commandList, desc);
}

std::unique_ptr<RayTracingShaderTable> D3D12RayTracingServices::CreateRayTracingShaderTable(const RayTracingShaderTableDesc& desc)
{
	if (m_rhi == nullptr || !m_rhi->GetRayTracingCapabilities().SupportsRayTracingPipeline)
	{
		throw Diagnostics::Error("D3D12 ray-tracing shader-table creation requires complete pipeline readiness.");
	}
	return std::make_unique<D3D12RayTracingShaderTable>(*m_rhi, *m_resourceService, desc);
}
