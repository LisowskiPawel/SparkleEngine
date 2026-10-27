#include "PCH.h"

#include "Panels/TitleBarWindowControls.h"

#include "Style/SparkleUiPalette.h"
#include "Window/Window.h"

#include <imgui.h>

static bool DrawTitleBarButton(const char* id, const ImVec2& size, const ImVec4& baseColor, const ImVec4& hoveredColor, const ImVec4& activeColor) noexcept
{
	ImGui::PushStyleColor(ImGuiCol_Button, baseColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
	const bool pressed = ImGui::Button(id, size);
	ImGui::PopStyleColor(3);
	return pressed;
}

static void DrawMinimizeIcon() noexcept
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImU32 iconColor = SparkleUiPalette::TitleBarIcon();
	const ImVec2 min = ImGui::GetItemRectMin();
	const ImVec2 max = ImGui::GetItemRectMax();
	const float y = min.y + ((max.y - min.y) * 0.68f);
	const float padding = (max.x - min.x) * 0.30f;
	drawList->AddLine(ImVec2(min.x + padding, y), ImVec2(max.x - padding, y), iconColor, 1.6f);
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
	drawList->AddRect(ImVec2(topLeft.x + offset, topLeft.y), ImVec2(bottomRight.x, bottomRight.y - offset), iconColor, 0.0f, 0, 1.2f);
	drawList->AddRect(ImVec2(topLeft.x, topLeft.y + offset), ImVec2(bottomRight.x - offset, bottomRight.y), iconColor, 0.0f, 0, 1.2f);
}

static void DrawCloseIcon() noexcept
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImU32 iconColor = SparkleUiPalette::TitleBarIcon();
	const ImVec2 min = ImGui::GetItemRectMin();
	const ImVec2 max = ImGui::GetItemRectMax();
	const float padding = (max.x - min.x) * 0.30f;
	drawList->AddLine(ImVec2(min.x + padding, min.y + padding), ImVec2(max.x - padding, max.y - padding), iconColor, 1.6f);
	drawList->AddLine(ImVec2(max.x - padding, min.y + padding), ImVec2(min.x + padding, max.y - padding), iconColor, 1.6f);
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
	ImGui::InvisibleButton("##TitleBarDragRegion", ImVec2(dragRegionWidth, buttonHeight));
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		window->ToggleMaximizeRestore();
	}
	else if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f))
	{
		window->BeginDragMove();
		ImGui::GetIO().MouseDown[ImGuiMouseButton_Left] = false;
	}

	ImGui::SameLine(0.0f, style.ItemSpacing.x);
	if (DrawTitleBarButton(
	        "##MinimizeWindow",
	        ImVec2(buttonWidth, buttonHeight),
	        SparkleUiPalette::TitleBarControlBackground(),
	        SparkleUiPalette::TitleBarControlBackgroundHovered(),
	        SparkleUiPalette::TitleBarControlBackgroundActive()))
	{
		window->Minimize();
	}
	DrawMinimizeIcon();

	ImGui::SameLine(0.0f, style.ItemSpacing.x);
	if (DrawTitleBarButton(
	        "##ToggleMaximizeWindow",
	        ImVec2(buttonWidth, buttonHeight),
	        SparkleUiPalette::TitleBarControlBackground(),
	        SparkleUiPalette::TitleBarControlBackgroundHovered(),
	        SparkleUiPalette::TitleBarControlBackgroundActive()))
	{
		window->ToggleMaximizeRestore();
	}
	DrawMaximizeIcon(*window);

	ImGui::SameLine(0.0f, style.ItemSpacing.x);
	if (DrawTitleBarButton(
	        "##CloseWindow",
	        ImVec2(buttonWidth, buttonHeight),
	        SparkleUiPalette::DangerBackground(),
	        SparkleUiPalette::DangerBackgroundHovered(),
	        SparkleUiPalette::DangerBackgroundActive()))
	{
		window->RequestClose();
	}
	DrawCloseIcon();
}
