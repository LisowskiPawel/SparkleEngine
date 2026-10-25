#pragma once

#include "Core/RhiCapabilities.h"

class D3D12Rhi;
class D3D12SwapChain;
class RenderDiagnostics;

RhiCapabilities BuildD3D12Capabilities(D3D12Rhi& rhi, D3D12SwapChain& swapChain, RenderDiagnostics& diagnostics) noexcept;
