#include "PCH.h"

#include "RayTracing/Acceleration/RayTracingAccelerationStrategy.h"

#include "RayTracing/RayTracingCapabilityReport.h"
#include "RayTracing/Acceleration/RayTracingTlasStrategy.h"
#include "RayTracing/Acceleration/RayTracingPtlasStrategy.h"

RayTracingAccelerationStrategy::RayTracingAccelerationStrategy() noexcept = default;

RayTracingAccelerationStrategy::~RayTracingAccelerationStrategy() noexcept = default;

std::unique_ptr<RayTracingAccelerationStrategy> CreateRayTracingAccelerationStrategy(RenderHardwareInterface& renderHardwareInterface, const RayTracingCapabilityReport& capabilityReport) noexcept
{
	if (capabilityReport.TlasProvider.SelectedProvider == ERhiRayTracingTlasProvider::Ptlas)
	{
		return std::make_unique<RayTracingPtlasStrategy>(renderHardwareInterface, capabilityReport);
	}

	return std::make_unique<RayTracingTlasStrategy>(renderHardwareInterface);
}
