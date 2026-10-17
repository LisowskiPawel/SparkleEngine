#pragma once

#include "Application.h"
#include "../../Renderer/Public/Viewport/ViewportContracts.h"

#include <cstdint>
#include <memory>
#include <string_view>

class Timer;
class Window;
class InputSystem;
class GameWorld;
class Renderer;
class LevelSession;
class GameUiHost;
class TaskExecutor;
class TaskScope;
class ApplicationTaskRuntime;
class CameraInputIntentCollector;
class EditorApplication;
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
class GpuCrashIncidentSink;
#endif
struct CameraInputIntent;
struct RenderViewCameraData;

enum class GameApplicationFrameResult : std::uint8_t
{
	Exit = 0,
	SkipRender = 1,
	Ready = 2,
};

struct GameApplicationOptions final
{
	bool EnableGameUi = true;
	bool EnableGameConsole = !SPARKLE_BUILD_SHIPPING;
	bool AllowThreadedRenderer = true;
	bool EnableUiRenderPackets = false;
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
	GameApplicationFrameResult BeginFrame();
	void UpdateGame() noexcept;
	void SubmitViewportRenderRequest(ViewportRenderRequest request) noexcept;
	Timer& GetTimer() noexcept;
	Window& GetWindow() noexcept;
	InputSystem& GetInputSystem() noexcept;
	LevelSession* GetLevelSession() const noexcept;
	Renderer& GetRenderer() noexcept;
	TaskExecutor& GetTaskExecutor() noexcept;
	TaskScope& GetApplicationTaskScope() noexcept;
	bool Tick() override;
	void Shutdown() override;

private:
	friend class EditorApplication;

	static bool WantsImGuiInputCapture() noexcept;
	void InitializeHost();
	void InitializeInput();
	void InitializeGameWorld();
	void InitializeRenderer();
	void InitializeGameUi();
	void SignalLevelReadiness(std::string_view level);
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	void ReportReadiness();
#endif
	CameraInputIntent CollectCameraInputIntent(float aspectRatio) noexcept;
	void UpdateGameFrame(const CameraInputIntent* worldCameraIntent, const RenderViewCameraData* renderCameraOverride) noexcept;
	void UpdateEditorGame(const RenderViewCameraData& renderCamera) noexcept;
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
	std::unique_ptr<GameUiHost> m_gameUiHost;
	std::unique_ptr<ApplicationTaskRuntime> m_taskRuntime;
	std::unique_ptr<CameraInputIntentCollector> m_cameraInputIntentCollector;
	GameApplicationOptions m_options;
	bool m_isInitialized = false;
};
