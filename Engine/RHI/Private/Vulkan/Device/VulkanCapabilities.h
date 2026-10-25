#pragma once

#include "Core/RhiCapabilities.h"

class VulkanRhi;
class VulkanSwapChain;
class RenderDiagnostics;

RhiCapabilities BuildVulkanCapabilities(VulkanRhi& rhi, VulkanSwapChain& swapChain, RenderDiagnostics& diagnostics, bool hasCommandRecordingContext) noexcept;
