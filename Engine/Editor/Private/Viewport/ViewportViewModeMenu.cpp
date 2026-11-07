#include "PCH.h"

#include "Viewport/ViewportViewModeMenu.h"

#include "Renderer/Public/Viewport/RenderViewMode.h"
#include "Icons/EditorIcon.h"
#include "Widgets/UiMenuWidgets.h"
#include "Viewport/EditorViewportSession.h"

#include <imgui.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>

struct ViewModePresentation final
{
	RenderViewMode Mode;
	EditorIcons::EditorIcon Icon;
	const char* Label;
	const char* Category;
};

static constexpr auto viewModePresentations = std::to_array<ViewModePresentation>(
    {{RenderViewMode::Lit, EditorIcons::EditorIcon::ViewLit, "Lit", ""},
        {RenderViewMode::ReferencePathTracer, EditorIcons::EditorIcon::ViewLit, "Reference Path Tracer", ""},
        {RenderViewMode::Wireframe, EditorIcons::EditorIcon::ViewMode, "Wireframe", ""},
        {RenderViewMode::GpuSceneInstances, EditorIcons::EditorIcon::ViewMode, "GPU Scene Instances", ""},
        {RenderViewMode::GBufferDiffuse, EditorIcons::EditorIcon::ViewDiffuse, "GBuffer Diffuse", "GBuffer"},
        {RenderViewMode::GBufferWorldNormal, EditorIcons::EditorIcon::ViewNormal, "GBuffer World Normal", "GBuffer"},
        {RenderViewMode::GBufferWorldTangent, EditorIcons::EditorIcon::ViewNormal, "GBuffer World Tangent", "GBuffer"},
        {RenderViewMode::GBufferRoughness, EditorIcons::EditorIcon::ViewRoughness, "GBuffer Roughness", "GBuffer"},
        {RenderViewMode::GBufferMetallic, EditorIcons::EditorIcon::ViewMetallic, "GBuffer Metallic", "GBuffer"},
        {RenderViewMode::GBufferEmissive, EditorIcons::EditorIcon::ViewEmissive, "GBuffer Emissive", "GBuffer"},
        {RenderViewMode::GBufferAmbientOcclusion, EditorIcons::EditorIcon::ViewAmbientOcclusion, "GBuffer Ambient Occlusion", "GBuffer"},
        {RenderViewMode::GBufferSubsurfaceColor, EditorIcons::EditorIcon::ViewSubsurfaceColor, "GBuffer Subsurface Color", "GBuffer"},
        {RenderViewMode::GBufferSubsurfaceStrength, EditorIcons::EditorIcon::ViewSubsurfaceStrength, "GBuffer Subsurface Strength", "GBuffer"},
        {RenderViewMode::DirectDiffuse, EditorIcons::EditorIcon::ViewDirectDiffuse, "Direct Diffuse", "Lighting"},
        {RenderViewMode::DirectSpecular, EditorIcons::EditorIcon::ViewDirectSpecular, "Direct Specular", "Lighting"},
        {RenderViewMode::DirectSubsurface, EditorIcons::EditorIcon::ViewDirectSubsurface, "Direct Subsurface", "Lighting"},
        {RenderViewMode::IndirectDiffuse, EditorIcons::EditorIcon::ViewDirectDiffuse, "Indirect Diffuse", "Lighting"},
        {RenderViewMode::IndirectSpecular, EditorIcons::EditorIcon::ViewDirectSpecular, "Indirect Specular", "Lighting"}});

static_assert(viewModePresentations.size() == static_cast<std::size_t>(RenderViewMode::Count));

static const ViewModePresentation& DescribeViewMode(RenderViewMode viewMode) noexcept
{
	const auto found = std::ranges::find(viewModePresentations, viewMode, &ViewModePresentation::Mode);
	return found != viewModePresentations.end() ? *found : viewModePresentations.front();
}

static void DrawViewModeOption(EditorViewportSession& viewportSession, const ViewModePresentation& option, RenderViewMode currentViewMode) noexcept
{
	const bool selected = option.Mode == currentViewMode;
	const EditorWidgets::MenuCheckState checkState = selected ? EditorWidgets::MenuCheckState::Checked : EditorWidgets::MenuCheckState::Unchecked;
	if (EditorWidgets::DrawMenuItem(option.Label, option.Icon, checkState))
	{
		viewportSession.SetViewMode(option.Mode);
	}

	if (selected)
	{
		ImGui::SetItemDefaultFocus();
	}
}

void DrawViewportViewModeMenu(EditorViewportSession& viewportSession, bool disableInteraction, bool compact) noexcept
{
	RenderViewMode currentViewMode = viewportSession.GetViewMode();
	if (currentViewMode >= RenderViewMode::Count)
	{
		currentViewMode = RenderViewMode::Lit;
	}

	ImGui::BeginDisabled(disableInteraction);
	const ViewModePresentation& currentPresentation = DescribeViewMode(currentViewMode);
	const std::string previewLabel = EditorIcons::MakeIconLabel(currentPresentation.Icon, currentPresentation.Label);
	const float previewWidth = ImGui::CalcTextSize(previewLabel.c_str()).x + ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.x * 2.0f;
	ImGui::SetNextItemWidth((std::clamp) (previewWidth, 100.0f, compact ? 145.0f : 260.0f));
	const EditorWidgets::MenuStyleScope menuStyle;
	if (ImGui::BeginCombo("##ViewportViewMode", previewLabel.c_str()))
	{
		for (const ViewModePresentation& option : viewModePresentations)
		{
			if (std::string_view(option.Category).empty())
			{
				DrawViewModeOption(viewportSession, option, currentViewMode);
			}
		}
		EditorWidgets::DrawMenuSection("VISUALIZATIONS");
		for (const char* category : {"GBuffer", "Lighting"})
		{
			float childWidth = 0.0f;
			for (const ViewModePresentation& option : viewModePresentations)
			{
				if (std::string_view(option.Category) == category)
				{
					childWidth = (std::max) (childWidth, EditorWidgets::MeasureMenuRow(option.Label, EditorWidgets::MenuRowKind::Toggle));
				}
			}
			if (EditorWidgets::BeginMenu(category, EditorIcons::EditorIcon::ViewMode, childWidth))
			{
				for (const ViewModePresentation& option : viewModePresentations)
				{
					if (std::string_view(option.Category) == category)
					{
						DrawViewModeOption(viewportSession, option, currentViewMode);
					}
				}
				ImGui::EndMenu();
			}
		}

		ImGui::EndCombo();
	}
	ImGui::EndDisabled();
}
