#pragma once

#include "Host/Application.h"

#include <cstdint>
#include <memory>
#include <stop_token>
#include <string>
#include <string_view>

class Timer;
class Window;
class InputSystem;
class GameWorld;
class Renderer;
class LevelSession;
struct enkiTaskScheduler;
class CameraInputIntentCollector;
class EditorApplication;
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
class GpuCrashIncidentSink;
#endif
struct RendererGraphicsLaunch;
struct CameraInputIntent;
struct RenderViewCameraData;
struct ViewportRenderRequest;

enum class GameApplicationFrameResult : std::uint8_t
{
	Exit = 0,
	SkipRender = 1,
	Ready = 2,
};

struct GameApplicationOptions final
{
	std::string WindowTitle = "Sparkle Game";
	bool bEnableUiRenderPackets = false;
};

class SPARKLE_APPLICATION_API GameApplication final : public Application
{
public:
	GameApplication();
	explicit GameApplication(GameApplicationOptions options) noexcept;
	~GameApplication();

	GameApplication(const GameApplication&) = delete;
	GameApplication& operator=(const GameApplication&) = delete;
	GameApplication(GameApplication&&) = delete;
	GameApplication& operator=(GameApplication&&) = delete;

	void Initialize() override;
	bool Tick() override;
	void Shutdown() override;

private:
	friend class EditorApplication;

	GameApplicationFrameResult BeginFrame();
	void UpdateGame();
	void SubmitViewportRenderRequest(ViewportRenderRequest request) noexcept;

	Timer& GetTimer() noexcept;
	Window& GetWindow() noexcept;
	InputSystem& GetInputSystem() noexcept;
	LevelSession* GetLevelSession() const noexcept;
	Renderer& GetRenderer() noexcept;

	void InitializeExecution();
	void ShutdownExecution() noexcept;
	void InitializeHost();
	void InitializeInput();
	void InitializeGameWorld();
	void InitializeRenderer();

	void PrepareGpuCrashDiagnostics(RendererGraphicsLaunch& graphics);
	void AbortGpuCrashDiagnostics() noexcept;
	void SignalLevelReadiness(std::string_view level);
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	void ReportReadiness();
#endif

	CameraInputIntent CollectCameraInputIntent(float aspectRatio) noexcept;
	void UpdateGameFrame(const CameraInputIntent* worldCameraIntent, const RenderViewCameraData* renderCameraOverride);
	void UpdateEditorGame(const RenderViewCameraData& renderCamera);
	void SubmitRenderFrame(std::uint64_t frameId, const RenderViewCameraData* renderCameraOverride);
	GameWorld& GetWorldForEditor() noexcept;

	std::unique_ptr<Timer> m_timer;
	std::unique_ptr<Window> m_window;
	std::unique_ptr<InputSystem> m_inputSystem;
	std::unique_ptr<GameWorld> m_gameWorld;
	std::unique_ptr<LevelSession> m_levelSession;
	std::unique_ptr<Renderer> m_renderer;
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	std::unique_ptr<GpuCrashIncidentSink> m_gpuCrashSink;
	std::string m_pendingReadiness;
#endif
	enkiTaskScheduler* m_frameScheduler = nullptr;
	enkiTaskScheduler* m_backgroundScheduler = nullptr;
	enkiTaskScheduler* m_ioScheduler = nullptr;
	std::stop_source m_stop{std::nostopstate};
	std::unique_ptr<CameraInputIntentCollector> m_cameraInputIntentCollector;
	GameApplicationOptions m_options;
	bool m_bIsInitialized = false;
};
