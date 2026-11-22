#include "PCH.h"

#include "Workspace/TitleBarWindowControls.h"

#include "Style/SparkleUiPalette.h"
#include "Window/Window.h"

#include <imgui.h>

static bool DrawTitleBarButton(const char* id, const ImVec2& size, const ImVec4& baseColor, const ImVec4& hoveredColor, const ImVec4& activeColor) noexcept
{
	ImGui::PushStyleColor(ImGuiCol_Button, baseColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
	const bool bPressed = ImGui::Button(id, size);
	ImGui::PopStyleColor(3);
	return bPressed;
}

static void DrawMinimizeIcon() noexcept
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImU32 iconColor = SparkleUiPalette::TitleBarIcon();
	const ImVec2 min = ImGui::GetItemRectMin();
	const ImVec2 max = ImGui::GetItemRectMax();
	const float y = min.y + ((max.y - min.y) * 0.68f);
	const float padding = (max.x - min.x) * 0.30f;
	const ImVec2 lineStart(min.x + padding, y);
	const ImVec2 lineEnd(max.x - padding, y);
	drawList->AddLine(lineStart, lineEnd, iconColor, 1.6f);
}

static void DrawMaximizeIcon(const Window& window) noexcept
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImU32 iconColor = SparkleUiPalette::TitleBarIcon();
	const ImVec2 min = ImGui::GetItemRectMin();
	const ImVec2 max = ImGui::GetItemRectMax();
	const float width = max.x - min.x;
	const float padding = width * 0.28f;
	const ImVec2 topLeft(min.x + padding, min.y + padding);
	const ImVec2 bottomRight(max.x - padding, max.y - padding);

	if (!window.IsMaximized())
	{
		drawList->AddRect(topLeft, bottomRight, iconColor, 0.0f, 0, 1.4f);
		return;
	}

	const float offset = width * 0.14f;
	const ImVec2 rearTopLeft(topLeft.x + offset, topLeft.y);
	const ImVec2 rearBottomRight(bottomRight.x, bottomRight.y - offset);
	drawList->AddRect(rearTopLeft, rearBottomRight, iconColor, 0.0f, 0, 1.2f);
	const ImVec2 frontTopLeft(topLeft.x, topLeft.y + offset);
	const ImVec2 frontBottomRight(bottomRight.x - offset, bottomRight.y);
	drawList->AddRect(frontTopLeft, frontBottomRight, iconColor, 0.0f, 0, 1.2f);
}

static void DrawCloseIcon() noexcept
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImU32 iconColor = SparkleUiPalette::TitleBarIcon();
	const ImVec2 min = ImGui::GetItemRectMin();
	const ImVec2 max = ImGui::GetItemRectMax();
	const float padding = (max.x - min.x) * 0.30f;
	const ImVec2 topLeft(min.x + padding, min.y + padding);
	const ImVec2 bottomRight(max.x - padding, max.y - padding);
	const ImVec2 topRight(max.x - padding, min.y + padding);
	const ImVec2 bottomLeft(min.x + padding, max.y - padding);
	drawList->AddLine(topLeft, bottomRight, iconColor, 1.6f);
	drawList->AddLine(topRight, bottomLeft, iconColor, 1.6f);
}

void DrawTitleBarWindowControls(Window* window) noexcept
{
	if (window == nullptr || window->IsFullScreen())
	{
		return;
	}

	const ImGuiStyle& style = ImGui::GetStyle();
	const float buttonHeight = ImGui::GetFrameHeight();
	const float buttonWidth = buttonHeight * 1.35f;
	const float controlsWidth = (buttonWidth * 3.0f) + (style.ItemSpacing.x * 2.0f);
	const float currentCursorX = ImGui::GetCursorPosX();
	const float rightEdgeX = ImGui::GetWindowWidth() - style.WindowPadding.x;
	float dragRegionWidth = rightEdgeX - currentCursorX - controlsWidth;
	if (dragRegionWidth < 1.0f)
	{
		dragRegionWidth = 1.0f;
	}

	ImGui::SameLine();
	const ImVec2 dragStart = ImGui::GetCursorScreenPos();
	window->SetTitleBarDragRegion(dragStart.x, dragStart.y, dragStart.x + dragRegionWidth, dragStart.y + buttonHeight);
	ImGui::Dummy(ImVec2(dragRegionWidth, buttonHeight));

	ImGui::SameLine(0.0f, style.ItemSpacing.x);

	const bool bMinimizeRequested = DrawTitleBarButton(
	    "##MinimizeWindow",
	    ImVec2(buttonWidth, buttonHeight),
	    SparkleUiPalette::TitleBarControlBackground(),
	    SparkleUiPalette::TitleBarControlBackgroundHovered(),
	    SparkleUiPalette::TitleBarControlBackgroundActive());

	if (bMinimizeRequested)
	{
		window->Minimize();
	}
	DrawMinimizeIcon();

	ImGui::SameLine(0.0f, style.ItemSpacing.x);

	const bool bMaximizeRequested = DrawTitleBarButton(
	    "##ToggleMaximizeWindow",
	    ImVec2(buttonWidth, buttonHeight),
	    SparkleUiPalette::TitleBarControlBackground(),
	    SparkleUiPalette::TitleBarControlBackgroundHovered(),
	    SparkleUiPalette::TitleBarControlBackgroundActive());

	if (bMaximizeRequested)
	{
		window->ToggleMaximizeRestore();
	}
	DrawMaximizeIcon(*window);

	ImGui::SameLine(0.0f, style.ItemSpacing.x);

	const bool bCloseRequested = DrawTitleBarButton(
	    "##CloseWindow",
	    ImVec2(buttonWidth, buttonHeight),
	    SparkleUiPalette::DangerBackground(),
	    SparkleUiPalette::DangerBackgroundHovered(),
	    SparkleUiPalette::DangerBackgroundActive());

	if (bCloseRequested)
	{
		window->RequestClose();
	}
	DrawCloseIcon();
}
