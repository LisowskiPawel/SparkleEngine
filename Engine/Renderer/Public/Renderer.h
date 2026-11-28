#pragma once

#include "RendererAPI.h"
#include "Concurrency/RendererExecutionConfig.h"

#include <cstdint>
#include <memory>

class Timer;
class Window;
class RendererFacadeState;
struct RenderFrameSubmission;
struct RendererGraphicsLaunch;
struct ViewportRenderRequest;
struct UiRenderPacket;
struct EngineRenderingSettingsState;
struct ViewportPresentationSnapshot;
struct CVarControlRequest;
struct CVarControlResult;
struct RendererMemoryDiagnosticsSnapshot;
struct ViewportCaptureAdmission;
struct ViewportCaptureRequest;
struct ViewportCaptureId;
struct ViewportCaptureReadback;
struct ExternalCaptureSnapshot;
struct ExternalCaptureAdmission;

class SPARKLE_RENDERER_API Renderer final
{
public:
	// Returns no renderer when the host closes during graphics preparation.
	static std::unique_ptr<Renderer> Create(Timer& timer, Window& window, RendererGraphicsLaunch graphicsLaunch, RendererExecutionConfig config = {});
	~Renderer() noexcept;

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	void SubmitViewportRenderRequest(ViewportRenderRequest request) noexcept;
	void SubmitRenderFrame(RenderFrameSubmission submission) noexcept;
	void SubmitUiRenderPacket(UiRenderPacket packet) noexcept;
	void SubmitRenderingSettings(EngineRenderingSettingsState settings) noexcept;
	void BeginSimulationFrame(std::uint64_t frameId) noexcept;
	void EndSimulationFrame(std::uint64_t frameId) noexcept;

	ViewportPresentationSnapshot GetViewportPresentation() const;
	EngineRenderingSettingsState CaptureRenderingSettings() const;
	CVarControlResult ExecuteConsoleVariables(CVarControlRequest request);

	void ReloadShaders();
	std::uint64_t GetShaderGeneration() const noexcept;
	RendererMemoryDiagnosticsSnapshot CaptureMemoryDiagnostics() const;
	ViewportCaptureAdmission RequestViewportCapture(ViewportCaptureRequest request) noexcept;
	bool TryTakeViewportCapture(ViewportCaptureId id, ViewportCaptureReadback& readback) noexcept;

	void OnRender() noexcept;
	ExternalCaptureSnapshot ObserveExternalCapture() const;
	ExternalCaptureAdmission RequestExternalCapture(std::uint64_t viewportGeneration) noexcept;

private:
	explicit Renderer(std::unique_ptr<RendererFacadeState> state) noexcept;

	std::unique_ptr<RendererFacadeState> m_state;
};
