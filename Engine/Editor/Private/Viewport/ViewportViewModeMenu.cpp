#include "PCH.h"

#include "Viewport/ViewportViewModeMenu.h"

#include "Renderer/Public/Viewport/RenderViewMode.h"
#include "Util/UiUtil.h"
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
	UiUtil::EditorIcon Icon;
	const char* Label;
	const char* Category;
};

static constexpr auto viewModePresentations = std::to_array<ViewModePresentation>(
    {{RenderViewMode::Lit, UiUtil::EditorIcon::ViewLit, "Lit", ""},
        {RenderViewMode::ReferencePathTracer, UiUtil::EditorIcon::ViewLit, "Reference Path Tracer", ""},
        {RenderViewMode::Wireframe, UiUtil::EditorIcon::ViewMode, "Wireframe", ""},
        {RenderViewMode::GpuSceneInstances, UiUtil::EditorIcon::ViewMode, "GPU Scene Instances", ""},
        {RenderViewMode::GBufferDiffuse, UiUtil::EditorIcon::ViewDiffuse, "GBuffer Diffuse", "GBuffer"},
        {RenderViewMode::GBufferWorldNormal, UiUtil::EditorIcon::ViewNormal, "GBuffer World Normal", "GBuffer"},
        {RenderViewMode::GBufferWorldTangent, UiUtil::EditorIcon::ViewNormal, "GBuffer World Tangent", "GBuffer"},
        {RenderViewMode::GBufferRoughness, UiUtil::EditorIcon::ViewRoughness, "GBuffer Roughness", "GBuffer"},
        {RenderViewMode::GBufferMetallic, UiUtil::EditorIcon::ViewMetallic, "GBuffer Metallic", "GBuffer"},
        {RenderViewMode::GBufferEmissive, UiUtil::EditorIcon::ViewEmissive, "GBuffer Emissive", "GBuffer"},
        {RenderViewMode::GBufferAmbientOcclusion, UiUtil::EditorIcon::ViewAmbientOcclusion, "GBuffer Ambient Occlusion", "GBuffer"},
        {RenderViewMode::GBufferSubsurfaceColor, UiUtil::EditorIcon::ViewSubsurfaceColor, "GBuffer Subsurface Color", "GBuffer"},
        {RenderViewMode::GBufferSubsurfaceStrength, UiUtil::EditorIcon::ViewSubsurfaceStrength, "GBuffer Subsurface Strength", "GBuffer"},
        {RenderViewMode::DirectDiffuse, UiUtil::EditorIcon::ViewDirectDiffuse, "Direct Diffuse", "Lighting"},
        {RenderViewMode::DirectSpecular, UiUtil::EditorIcon::ViewDirectSpecular, "Direct Specular", "Lighting"},
        {RenderViewMode::DirectSubsurface, UiUtil::EditorIcon::ViewDirectSubsurface, "Direct Subsurface", "Lighting"},
        {RenderViewMode::IndirectDiffuse, UiUtil::EditorIcon::ViewDirectDiffuse, "Indirect Diffuse", "Lighting"},
        {RenderViewMode::IndirectSpecular, UiUtil::EditorIcon::ViewDirectSpecular, "Indirect Specular", "Lighting"}});

static_assert(viewModePresentations.size() == static_cast<std::size_t>(RenderViewMode::Count));

static const ViewModePresentation& DescribeViewMode(RenderViewMode viewMode) noexcept
{
	const auto found = std::ranges::find(viewModePresentations, viewMode, &ViewModePresentation::Mode);
	return found != viewModePresentations.end() ? *found : viewModePresentations.front();
}

static void DrawViewModeOption(EditorViewportSession& viewportSession, const ViewModePresentation& option, RenderViewMode currentViewMode) noexcept
{
	const bool selected = option.Mode == currentViewMode;
	const UiUtil::MenuCheckState checkState = selected ? UiUtil::MenuCheckState::Checked : UiUtil::MenuCheckState::Unchecked;
	if (UiUtil::DrawMenuItem(option.Label, option.Icon, checkState))
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
	const std::string previewLabel = UiUtil::MakeIconLabel(currentPresentation.Icon, currentPresentation.Label);
	const float previewWidth = ImGui::CalcTextSize(previewLabel.c_str()).x + ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.x * 2.0f;
	ImGui::SetNextItemWidth((std::clamp) (previewWidth, 100.0f, compact ? 145.0f : 260.0f));
	const UiUtil::MenuStyleScope menuStyle;
	if (ImGui::BeginCombo("##ViewportViewMode", previewLabel.c_str()))
	{
		for (const ViewModePresentation& option : viewModePresentations)
		{
			if (std::string_view(option.Category).empty())
			{
				DrawViewModeOption(viewportSession, option, currentViewMode);
			}
		}
		UiUtil::DrawMenuSection("VISUALIZATIONS");
		for (const char* category : {"GBuffer", "Lighting"})
		{
			float childWidth = 0.0f;
			for (const ViewModePresentation& option : viewModePresentations)
			{
				if (std::string_view(option.Category) == category)
				{
					childWidth = (std::max) (childWidth, UiUtil::MeasureMenuRow(option.Label, UiUtil::MenuRowKind::Toggle));
				}
			}
			if (UiUtil::BeginMenu(category, UiUtil::EditorIcon::ViewMode, childWidth))
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
