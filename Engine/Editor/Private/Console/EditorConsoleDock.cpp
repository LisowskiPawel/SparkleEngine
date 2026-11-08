#include "PCH.h"

#include "Console/EditorConsolePanel.h"
#include "Style/SparkleUiPalette.h"
#include "Icons/EditorIcon.h"
#include "Widgets/UiPanelWidgets.h"

#include <imgui.h>

#include <algorithm>

static constexpr float MinimumDockHeight = 160.0f;
static constexpr float MinimumViewportHeight = 64.0f;

void EditorConsolePanel::ToggleOpen() noexcept
{
	if (m_bIsOpen)
	{
		m_bIsOpen = false;
	}
	else
	{
		RequestFocus();
	}
}

float EditorConsolePanel::GetDockHeight(float availableHeight) noexcept
{
	const float maxDockHeight = (std::max) (0.0f, availableHeight - MinimumViewportHeight);
	if (!m_bIsOpen)
	{
		return (std::min) (ImGui::GetFrameHeight() + 12.0f, maxDockHeight);
	}
	if (maxDockHeight <= 0.0f)
	{
		m_dockHeight = 0.0f;
		return m_dockHeight;
	}

	const float minDockHeight = (std::min) (MinimumDockHeight, maxDockHeight);
	m_dockHeight = (std::clamp) (m_dockHeight, minDockHeight, maxDockHeight);
	return m_dockHeight;
}

void EditorConsolePanel::BuildDockedUI(float left, float bottom, float width, float availableHeight, bool bDisableInteraction)
{
	const float height = GetDockHeight(availableHeight);
	if (width <= 0.0f || height <= 0.0f)
	{
		return;
	}

	if (BeginDockWindow(left, bottom, width, availableHeight, height))
	{
		// Keep content admission consistent with the height chosen before a header toggle.
		const bool bShowContent = m_bIsOpen;
		DrawDockHeader(bDisableInteraction);
		if (bShowContent)
		{
			ImGui::Separator();
			BuildContent(bDisableInteraction);
			UpdateDockInteraction(availableHeight, bDisableInteraction);
		}
	}
	EndDockWindow();
}

bool EditorConsolePanel::BeginDockWindow(float left, float bottom, float width, float availableHeight, float height)
{
	const float maxDockHeight = (std::max) (0.0f, availableHeight - MinimumViewportHeight);
	const float minDockHeight = (std::min) (MinimumDockHeight, maxDockHeight);
	ImGui::SetNextWindowPos(ImVec2(left, bottom - height), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
	ImGui::SetNextWindowSizeConstraints(ImVec2(width, m_bIsOpen ? minDockHeight : height), ImVec2(width, m_bIsOpen ? maxDockHeight : height));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.0f, 4.0f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, SparkleUiPalette::WindowBackground());
	ImGui::PushStyleColor(ImGuiCol_Tab, SparkleUiPalette::TabBackground());
	ImGui::PushStyleColor(ImGuiCol_TabHovered, SparkleUiPalette::TabBackgroundHovered());
	ImGui::PushStyleColor(ImGuiCol_TabActive, SparkleUiPalette::TabBackgroundActive());

	const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings
	    | (m_bIsOpen ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoResize);

	return ImGui::Begin("Viewport Console Dock", nullptr, windowFlags);
}

void EditorConsolePanel::EndDockWindow()
{
	ImGui::End();
	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar(2);
}

void EditorConsolePanel::DrawDockHeader(bool bDisableInteraction)
{
	ImGui::BeginDisabled(bDisableInteraction);
	const bool bOpen = m_bIsOpen;
	const std::string consoleLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Console, "Console");
	if (EditorWidgets::DrawFilterChip(consoleLabel.c_str(), bOpen))
	{
		ToggleOpen();
	}
	ImGui::SameLine();
	ImGui::AlignTextToFramePadding();
	ImGui::TextDisabled(bOpen ? "Esc to close" : "~ to open");
	ImGui::EndDisabled();
}

void EditorConsolePanel::UpdateDockInteraction(float availableHeight, bool bDisableInteraction)
{
	m_dockHeight = ImGui::GetWindowHeight();
	(void) GetDockHeight(availableHeight);
	const bool bFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
	if (bDisableInteraction || !bFocused)
	{
		return;
	}

	const bool bPopupOpen = ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId);
	const bool bCloseRequested = !bPopupOpen && ImGui::IsKeyPressed(ImGuiKey_Escape);
	if (bCloseRequested)
	{
		m_bIsOpen = false;
	}
}
