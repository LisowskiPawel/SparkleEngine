#pragma once

#include <d3d12.h>
#include <wrl/client.h>

class D3D12Rhi;
struct RayTracingPipelineDesc;

void CreateD3D12RayTracingPipeline(
    D3D12Rhi& rhi,
    const RayTracingPipelineDesc& desc,
    Microsoft::WRL::ComPtr<ID3D12StateObject>& stateObject,
    Microsoft::WRL::ComPtr<ID3D12StateObjectProperties>& properties);
