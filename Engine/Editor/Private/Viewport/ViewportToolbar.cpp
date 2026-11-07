#include "PCH.h"
#include "Viewport/ViewportToolbar.h"

#include "Style/SparkleUiPalette.h"
#include "Icons/EditorIcon.h"
#include "Viewport/ViewportCameraProperties.h"
#include "Viewport/EditorViewportSession.h"
#include "Viewport/ViewportShowMenu.h"
#include "Viewport/ViewportViewModeMenu.h"

#include <imgui.h>

#include <algorithm>
#include <cstdio>
#include <string>
#include <string_view>

ViewportToolbar::ViewportToolbar(EditorViewportSession& viewportSession, const EngineRenderingSettingsState& renderingDefaults, const CVarControlExecutor& consoleVariables) noexcept :
    m_viewportSession(viewportSession),
    m_renderingDefaults(renderingDefaults),
    m_consoleVariables(consoleVariables)
{
}

ViewportToolbar::~ViewportToolbar() noexcept = default;

void ViewportToolbar::SetGeometry(float leftPixels, float topPixels, float widthPixels) noexcept
{
	m_leftPixels = leftPixels;
	m_topPixels = topPixels;
	m_widthPixels = widthPixels;
}

void ViewportToolbar::DrawLevelName(std::string_view levelName, bool bCompact) const noexcept
{
	const std::string activeLevelName(levelName);

	ImGui::AlignTextToFramePadding();
	if (bCompact)
	{
		const std::string compactLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Level, activeLevelName.c_str());
		ImGui::TextUnformatted(compactLabel.c_str());
		return;
	}

	const std::string levelLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Level, "Level");
	ImGui::TextDisabled("%s", levelLabel.c_str());
	ImGui::SameLine();
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(activeLevelName.c_str());
}

void ViewportToolbar::DrawCameraControls(bool bDisableInteraction, bool bCompact) noexcept
{
	const CameraProjectionKind projectionKind = m_viewportSession.GetSettings().ProjectionKind;
	const char* projectionLabel = projectionKind == CameraProjectionKind::Orthographic ? "Orthographic" : "Perspective";
	const std::string cameraText = bCompact ? EditorIcons::GetEditorIconGlyph(EditorIcons::EditorIcon::Camera) : EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Camera, projectionLabel);
	const std::string cameraLabel = cameraText + "##ViewportCameraPropertiesButton";

	ImGui::BeginDisabled(bDisableInteraction);
	if (ImGui::Button(cameraLabel.c_str()))
	{
		ViewportCameraProperties::OpenPopup();
	}
	if (bCompact && ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Camera properties (%s)", projectionLabel);
	}
	ImGui::EndDisabled();
	ViewportCameraProperties::BuildPopup(m_viewportSession, m_renderingDefaults, bDisableInteraction);
}

void ViewportToolbar::DrawFrameStats() const noexcept
{
	const ImGuiIO& io = ImGui::GetIO();
	char statsText[64] = {};
	std::snprintf(statsText, sizeof(statsText), "%.1f FPS  %.2f ms", io.Framerate, io.DeltaTime * 1000.0f);
	const float rightAlignedX = ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x - ImGui::CalcTextSize(statsText).x;
	if (rightAlignedX > ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x)
	{
		ImGui::SameLine(rightAlignedX);
		ImGui::AlignTextToFramePadding();
		ImGui::TextDisabled("%s", statsText);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Editor frame rate and frame interval");
		}
	}
}

void ViewportToolbar::Draw(std::string_view levelName, bool bDisableInteraction) noexcept
{
	if (!BeginToolbarWindow())
	{
		return;
	}
	DrawToolbarContents(levelName, bDisableInteraction);
	DrawToolbarBorder();
	EndToolbarWindow();
}

bool ViewportToolbar::BeginToolbarWindow() noexcept
{
	if (m_widthPixels <= 0.0f)
	{
		m_heightPixels = 0.0f;
		return false;
	}

	const ImVec2 windowPadding(10.0f, 4.0f);
	m_heightPixels = ImGui::GetFrameHeight() + (windowPadding.y * 2.0f);

	ImGui::SetNextWindowPos(ImVec2(m_leftPixels, m_topPixels), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(m_widthPixels, m_heightPixels), ImGuiCond_Always);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, windowPadding);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, SparkleUiPalette::TitleBarBackground());
	ImGui::PushStyleColor(ImGuiCol_FrameBg, SparkleUiPalette::FrameBackground());
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, SparkleUiPalette::FrameBackgroundHovered());
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, SparkleUiPalette::FrameBackgroundActive());
	ImGui::PushStyleColor(ImGuiCol_Header, SparkleUiPalette::HeaderBackground());
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, SparkleUiPalette::HeaderBackgroundHovered());
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, SparkleUiPalette::HeaderBackgroundActive());

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoScrollbar;
	windowFlags |= ImGuiWindowFlags_NoScrollWithMouse;
	windowFlags |= ImGuiWindowFlags_NoSavedSettings;

	if (!ImGui::Begin("Viewport Toolbar", nullptr, windowFlags))
	{
		EndToolbarWindow();
		return false;
	}

	return true;
}

void ViewportToolbar::DrawToolbarContents(std::string_view levelName, bool bDisableInteraction) noexcept
{
	const bool bCompactToolbar = m_widthPixels < 760.0f;

	const bool bShowLevel = m_widthPixels >= 480.0f;
	if (bShowLevel)
	{
		DrawLevelName(levelName, bCompactToolbar);
		ImGui::SameLine(0.0f, bCompactToolbar ? 8.0f : 14.0f);
		ImGui::AlignTextToFramePadding();
		ImGui::TextDisabled("|");
		ImGui::SameLine(0.0f, bCompactToolbar ? 8.0f : 14.0f);
	}
	DrawCameraControls(bDisableInteraction, bCompactToolbar);
	ImGui::SameLine();
	DrawViewportViewModeMenu(m_viewportSession, bDisableInteraction, bCompactToolbar);
	ImGui::SameLine();
	DrawViewportShowMenu(&m_consoleVariables, bDisableInteraction, m_showControlError);
	if (!bCompactToolbar)
	{
		DrawFrameStats();
	}
}

void ViewportToolbar::DrawToolbarBorder() const noexcept
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImVec2 windowMin = ImGui::GetWindowPos();
	const ImVec2 windowMax(windowMin.x + ImGui::GetWindowWidth(), windowMin.y + ImGui::GetWindowHeight());
	drawList->AddLine(ImVec2(windowMin.x, windowMax.y - 1.0f), ImVec2(windowMax.x, windowMax.y - 1.0f), SparkleUiPalette::PanelHeaderBorder(), 1.0f);
}

void ViewportToolbar::EndToolbarWindow() const noexcept
{
	ImGui::End();
	ImGui::PopStyleColor(7);
	ImGui::PopStyleVar(2);
}
