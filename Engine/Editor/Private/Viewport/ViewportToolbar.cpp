#include "PCH.h"
#include "Viewport/ViewportToolbar.h"

#include "Style/SparkleUiPalette.h"
#include "Util/UiUtil.h"
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

void ViewportToolbar::DrawLevelName(std::string_view levelName, bool compact) const noexcept
{
	const std::string activeLevelName(levelName);

	ImGui::AlignTextToFramePadding();
	if (compact)
	{
		const std::string compactLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Level, activeLevelName.c_str());
		ImGui::TextUnformatted(compactLabel.c_str());
		return;
	}

	const std::string levelLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Level, "Level");
	ImGui::TextDisabled("%s", levelLabel.c_str());
	ImGui::SameLine();
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(activeLevelName.c_str());
}

void ViewportToolbar::DrawCameraControls(bool disableInteraction, bool compact) noexcept
{
	const CameraProjectionKind projectionKind = m_viewportSession.GetSettings().ProjectionKind;
	const char* projectionLabel = projectionKind == CameraProjectionKind::Orthographic ? "Orthographic" : "Perspective";
	const std::string cameraText = compact ? UiUtil::GetEditorIconGlyph(UiUtil::EditorIcon::Camera) : UiUtil::MakeIconLabel(UiUtil::EditorIcon::Camera, projectionLabel);
	const std::string cameraLabel = cameraText + "##ViewportCameraPropertiesButton";

	ImGui::BeginDisabled(disableInteraction);
	if (ImGui::Button(cameraLabel.c_str()))
	{
		ViewportCameraProperties::OpenPopup();
	}
	if (compact && ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Camera properties (%s)", projectionLabel);
	}
	ImGui::EndDisabled();
	ViewportCameraProperties::BuildPopup(m_viewportSession, m_renderingDefaults, disableInteraction);
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

void ViewportToolbar::Draw(std::string_view levelName, bool disableInteraction) noexcept
{
	if (!BeginToolbarWindow())
	{
		return;
	}
	DrawToolbarContents(levelName, disableInteraction);
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

void ViewportToolbar::DrawToolbarContents(std::string_view levelName, bool disableInteraction) noexcept
{
	const bool compactToolbar = m_widthPixels < 760.0f;

	const bool showLevel = m_widthPixels >= 480.0f;
	if (showLevel)
	{
		DrawLevelName(levelName, compactToolbar);
		ImGui::SameLine(0.0f, compactToolbar ? 8.0f : 14.0f);
		ImGui::AlignTextToFramePadding();
		ImGui::TextDisabled("|");
		ImGui::SameLine(0.0f, compactToolbar ? 8.0f : 14.0f);
	}
	DrawCameraControls(disableInteraction, compactToolbar);
	ImGui::SameLine();
	DrawViewportViewModeMenu(m_viewportSession, disableInteraction, compactToolbar);
	ImGui::SameLine();
	DrawViewportShowMenu(&m_consoleVariables, disableInteraction, m_showControlError);
	if (!compactToolbar)
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
