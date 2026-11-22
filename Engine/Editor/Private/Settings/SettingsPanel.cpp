#include "PCH.h"

#include "Settings/SettingsPanel.h"

#include "Settings/Rendering/RenderingSettingsPanel.h"
#include "Settings/EngineRenderingSettingsController.h"
#include "Icons/EditorIcon.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <algorithm>
#include <cfloat>
#include <string>
#include <utility>

SettingsPanel::SettingsPanel() = default;
SettingsPanel::~SettingsPanel() = default;

void SettingsPanel::SetOpen(bool bOpen) noexcept
{
	if (bOpen && !m_bIsOpen)
	{
		m_bRefreshFromRuntimeOnNextOpen = true;
	}
	m_bIsOpen = bOpen;
}

void SettingsPanel::SetRenderingSettings(EngineRenderingSettingsController* renderingSettings) noexcept
{
	m_renderingSettings = renderingSettings;
	if (!m_renderingPanel)
	{
		m_renderingPanel = std::make_unique<RenderingSettingsPanel>();
	}
	m_renderingPanel->SetSettings(renderingSettings);
}

void SettingsPanel::SetRestartHandler(std::function<void()> restartHandler)
{
	m_restartHandler = std::move(restartHandler);
}

void SettingsPanel::BuildUI(bool bDisableInteraction)
{
	if (!m_bIsOpen || m_renderingSettings == nullptr)
	{
		return;
	}

	if (m_bRefreshFromRuntimeOnNextOpen)
	{
		m_renderingPanel->RefreshFromRuntimeState();
		m_bRefreshFromRuntimeOnNextOpen = false;
	}

	if (!BeginWindow())
	{
		return;
	}

	const bool bShowRestartBar = HasPendingRestart();
	const float restartBarHeight = bShowRestartBar ? 34.0f : 0.0f;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 10.0f));
	ImGui::BeginChild("##SettingsContent", ImVec2(0.0f, -restartBarHeight));
	DrawToolbar(bDisableInteraction);
	ImGui::Dummy(ImVec2(0.0f, 4.0f));
	m_renderingPanel->BuildUI(bDisableInteraction, m_filterText.c_str());
	ImGui::EndChild();

	if (bShowRestartBar)
	{
		DrawRestartBar(restartBarHeight);
	}

	ImGui::PopStyleVar();

	ImGui::End();
}

bool SettingsPanel::BeginWindow()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSize(ImVec2(1120.0f, 780.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 0.0f), viewport->WorkSize);
	const std::string windowTitle = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Settings, "Settings") + "##EditorSettings";
	if (!ImGui::Begin(windowTitle.c_str(), &m_bIsOpen))
	{
		ImGui::End();
		return false;
	}

	const ImVec2 windowSize = ImGui::GetWindowSize();
	const ImVec2 windowPosition = ImGui::GetWindowPos();
	const float rightAlignedX = viewport->WorkPos.x + viewport->WorkSize.x - windowSize.x;
	const float bottomAlignedY = viewport->WorkPos.y + viewport->WorkSize.y - windowSize.y;
	const float maximumX = (std::max) (viewport->WorkPos.x, rightAlignedX);
	const float maximumY = (std::max) (viewport->WorkPos.y, bottomAlignedY);
	const float constrainedX = std::clamp(windowPosition.x, viewport->WorkPos.x, maximumX);
	const float constrainedY = std::clamp(windowPosition.y, viewport->WorkPos.y, maximumY);
	const ImVec2 constrainedPosition(constrainedX, constrainedY);
	if (constrainedPosition.x != windowPosition.x || constrainedPosition.y != windowPosition.y)
	{
		ImGui::SetWindowPos(constrainedPosition);
	}

	return true;
}

void SettingsPanel::DrawRestartBar(float restartBarHeight)
{
	ImGui::Separator();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 4.0f));
	ImGui::BeginChild("##SettingsFooter", ImVec2(0.0f, restartBarHeight - 6.0f));
	ImGui::BeginDisabled(!m_restartHandler);
	const float buttonWidth = 96.0f;
	const float availableWidth = ImGui::GetContentRegionAvail().x;
	const float buttonX = (std::max) (0.0f, availableWidth - buttonWidth);
	ImGui::SetCursorPosX(buttonX);
	if (ImGui::Button("Restart", ImVec2(buttonWidth, 0.0f)) && m_restartHandler)
	{
		m_restartHandler();
	}
	ImGui::EndDisabled();
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void SettingsPanel::DrawToolbar(bool bDisableInteraction)
{
	ImGui::BeginDisabled(bDisableInteraction);
	const std::string resetLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Reset, "Reset rendering defaults");
	if (ImGui::Button(resetLabel.c_str()))
	{
		m_renderingSettings->ResetDefaults();
	}
	ImGui::EndDisabled();
	ImGui::Dummy(ImVec2(0.0f, 4.0f));

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
	ImGui::SetNextItemWidth(-FLT_MIN);
	const std::string searchHint = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Search, "Search");
	ImGui::InputTextWithHint("##SettingsSearch", searchHint.c_str(), &m_filterText);
	ImGui::PopStyleVar();
}

bool SettingsPanel::HasPendingRestart() const noexcept
{
	return m_renderingPanel != nullptr && m_renderingPanel->HasPendingRestart();
}
