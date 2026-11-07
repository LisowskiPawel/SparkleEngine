#include "PCH.h"
#include "UI.h"
#include "UIImplementation.h"

#include <utility>

UI::UI(EditorHostServices hostServices) :
    m_implementation(std::make_unique<Implementation>(std::move(hostServices)))
{
}

UI::~UI() noexcept = default;

const ViewportRenderRequest& UI::GetViewportRenderRequest() const noexcept
{
	return m_implementation->GetViewportRenderRequest();
}

RenderViewCameraData UI::UpdateViewportCamera(const CameraInputIntent& intent, float deltaSeconds) noexcept
{
	return m_implementation->UpdateViewportCamera(intent, deltaSeconds);
}

void UI::SetViewportOverlay(std::unique_ptr<ViewportOverlay> overlay) noexcept
{
	m_implementation->SetViewportOverlay(std::move(overlay));
}

void UI::SetViewportRenderProducts(const ViewportRenderProducts& products) noexcept
{
	m_implementation->SetViewportRenderProducts(products);
}

void UI::SetViewportFinalColorTexture(UiTextureHandle texture) noexcept
{
	m_implementation->SetViewportFinalColorTexture(texture);
}

void UI::SetDiagnosticsProviders(EditorDiagnosticsProviders providers)
{
	m_implementation->SetDiagnosticsProviders(std::move(providers));
}

RendererMemoryDiagnosticsSnapshot UI::CaptureMemoryDiagnostics() const
{
	return m_implementation->CaptureMemoryDiagnostics();
}

void UI::SetShaderCompileHandler(std::function<ConsoleCommandResult(bool)> handler)
{
	m_implementation->SetShaderCompileHandler(std::move(handler));
}

void UI::SetShaderCompileStatus(bool busy, std::string_view message)
{
	m_implementation->SetShaderCompileStatus(busy, message);
}

void UI::AppendConsoleOutput(ConsoleOutputRecord record, bool showConsole)
{
	m_implementation->AppendConsoleOutput(std::move(record), showConsole);
}

bool UI::ConsumeShaderReloadRequest() noexcept
{
	return m_implementation->ConsumeShaderReloadRequest();
}

ViewportOutputAction UI::ConsumeViewportOutputAction() noexcept
{
	return m_implementation->ConsumeViewportOutputAction();
}

UiRenderPacket UI::ConsumeRenderPacket()
{
	return m_implementation->ConsumeRenderPacket();
}

void UI::Update()
{
	m_implementation->Update();
}
