#pragma once

#include "Renderer/Public/Settings/EngineRenderingSettings.h"

#include <functional>
#include <string>

class EngineRenderingSettingsController final
{
public:
	using CommitHandler = std::function<void(EngineRenderingSettingsState)>;

	using RefreshHandler = std::function<EngineRenderingSettingsState()>;

	EngineRenderingSettingsController(EngineRenderingSettingsState state, CommitHandler commitHandler, RefreshHandler refreshHandler);

	const EngineRenderingSettingsState& GetState() const noexcept { return m_state; }

	void RefreshFromRuntimeState() noexcept;

	bool HasPendingRestart() const noexcept;

	const std::string& GetLastError() const noexcept { return m_lastError; }

	std::string BuildPendingRestartMessage() const;

	void SetVSync(bool bEnabled);
	void ResetDefaults();
	void SetBackBufferFormat(PixelFormat format);
	void SetPreferHighPerformanceAdapter(bool bEnabled);
	void SetToneMapper(EngineToneMapper toneMapper);
	void SetExposureMode(EngineExposureMode mode);
	void SetExposureMeteringMethod(EngineExposureMeteringMethod method);
	void SetOutputColorEncoding(EngineOutputColorEncoding encoding);
	void SetManualExposure(float exposure);
	void SetExposureCompensation(float compensation);
	void SetExposureTargetLuminance(float luminance);
	void SetExposureMin(float exposure);
	void SetExposureMax(float exposure);
	void SetExposureAdaptationSpeedUp(float speed);
	void SetExposureAdaptationSpeedDown(float speed);
	void SetUpscalerProvider(EUpscalerProviderKind provider);
	void SetUpscalerQualityMode(EUpscalerQualityMode mode);
	void SetRayReconstructionMode(EngineRayReconstructionMode mode);
	void SetGBufferAlgorithm(GBufferAlgorithm algorithm);
	void SetMeshAutoBatching(bool bEnabled);
	void SetRefitTlas(bool bEnabled);
	void SetPtlasActive(bool bActive);
	void SetPtlasPartitionsPerAxis(std::uint32_t partitionsPerAxis);
	void SetPtlasPartitionUpdateMode(RayTracingPtlasPartitionUpdateMode mode);
	void SetPtlasMarkAllDynamicInPartition(bool bEnabled);
	void SetPtlasModeChangeDistance(float distance);
#if SPARKLE_TARGET_EDITOR && !SPARKLE_BUILD_SHIPPING
	void SetStartupCaptureProvider(ExternalCaptureProvider provider);
#endif

private:
	template <typename TValue> void SetValue(TValue& destination, TValue value)
	{
		if (destination == value)
		{
			return;
		}
		const TValue previous = destination;
		destination = value;
		if (!CommitState())
		{
			destination = previous;
		}
	}

	bool CommitState();
	bool ComputePendingRestart() const noexcept;
	std::string DescribePendingRestart() const;

	EngineRenderingSettingsState m_state{};
	std::string m_lastError;
	PixelFormat m_sessionBackBufferFormat = RhiPresentationDefaults::DefaultBackBufferFormat;
	bool m_bSessionPreferHighPerformanceAdapter = true;
#if SPARKLE_TARGET_EDITOR && !SPARKLE_BUILD_SHIPPING
	ExternalCaptureProvider m_sessionStartupCaptureProvider = ExternalCaptureProvider::None;
#endif
	CommitHandler m_commitHandler;
	RefreshHandler m_refreshHandler;
};
