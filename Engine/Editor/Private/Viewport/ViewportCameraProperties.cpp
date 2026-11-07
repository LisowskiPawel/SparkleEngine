#include "PCH.h"

#include "Viewport/ViewportCameraProperties.h"

#include "Settings/Rendering/ExposureSettingsEditor.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Viewport/EditorViewportSession.h"

#include <imgui.h>

#include <cfloat>

namespace ViewportCameraProperties
{
	bool BeginPropertiesTable() noexcept
	{
		if (!ImGui::BeginTable("##ViewportCameraCore", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH))
		{
			return false;
		}
		ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 175.0f);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
		return true;
	}

	void BeginPropertyRow(const char* label) noexcept
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::TableSetColumnIndex(1);
	}

	bool DrawFloat(const char* id, const char* label, float& value, float speed, float minimum, float maximum, const char* format, ImGuiSliderFlags flags = ImGuiSliderFlags_None) noexcept
	{
		BeginPropertyRow(label);
		ImGui::SetNextItemWidth(-FLT_MIN);
		return ImGui::DragFloat(id, &value, speed, minimum, maximum, format, flags);
	}

	void DrawProjection(EditorViewportSession& viewportSession) noexcept
	{
		const auto& settings = viewportSession.GetSettings();
		BeginPropertyRow("Projection");
		ImGui::SetNextItemWidth(-FLT_MIN);
		const char* projectionPreview = settings.ProjectionKind == CameraProjectionKind::Orthographic ? "Orthographic" : "Perspective";
		if (ImGui::BeginCombo("##ViewportProjection", projectionPreview))
		{
			if (ImGui::Selectable("Perspective", settings.ProjectionKind == CameraProjectionKind::Perspective))
			{
				viewportSession.SetProjectionKind(CameraProjectionKind::Perspective);
			}
			if (ImGui::Selectable("Orthographic", settings.ProjectionKind == CameraProjectionKind::Orthographic))
			{
				viewportSession.SetProjectionKind(CameraProjectionKind::Orthographic);
			}
			ImGui::EndCombo();
		}
	}

	void DrawNavigation(EditorViewportSession& viewportSession) noexcept
	{
		const auto& settings = viewportSession.GetSettings();
		float moveSpeed = settings.Navigation.MoveSpeedMetersPerSecond;
		const bool bMoveSpeedEdited = DrawFloat(
				        "##ViewportMoveSpeed",
				        "Move speed (m/s)",
				        moveSpeed,
				        0.01f,
				        settings.Navigation.MinimumMoveSpeedMetersPerSecond,
				        settings.Navigation.MaximumMoveSpeedMetersPerSecond,
				        "%.4f",
				        ImGuiSliderFlags_Logarithmic);
		
		if (bMoveSpeedEdited)
		{
			viewportSession.SetMoveSpeed(moveSpeed);
		}

		float rotationSpeed = settings.Navigation.RotationSpeedDegreesPerPixel;
		const bool bRotationSpeedEdited = DrawFloat("##ViewportRotationSpeed", "Rotation speed (deg/px)", rotationSpeed, 0.005f, 0.001f, 10.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
		
		if (bRotationSpeedEdited)
		{
			viewportSession.SetRotationSpeed(rotationSpeed);
		}

		BeginPropertyRow("Invert Y");
		bool bInvertY = settings.Navigation.bInvertY;
		if (ImGui::Checkbox("##ViewportInvertY", &bInvertY))
		{
			viewportSession.SetInvertY(bInvertY);
		}
	}

	void DrawOrthographicSettings(EditorViewportSession& viewportSession) noexcept
	{
		const auto& settings = viewportSession.GetSettings();
		if (settings.ProjectionKind == CameraProjectionKind::Orthographic)
		{
			float orthographicHeight = settings.OrthographicHeightMeters;
			const bool bOrthographicHeightEdited = DrawFloat("##ViewportOrthographicHeight", "Ortho height (m)", orthographicHeight, 0.1f, 0.001f, 1000000.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
			
			if (bOrthographicHeightEdited)
			{
				viewportSession.SetOrthographicHeight(orthographicHeight);
			}
		}
	}

	void DrawCameraProperties(EditorViewportSession& viewportSession) noexcept
	{
		ImGui::TextDisabled("Viewport Camera");
		ImGui::Separator();
		if (!BeginPropertiesTable())
		{
			return;
		}
		DrawProjection(viewportSession);
		DrawNavigation(viewportSession);
		DrawOrthographicSettings(viewportSession);
		ImGui::EndTable();
	}

	void DrawExposureOverrides(EditorViewportSession& viewportSession, const EngineRenderingSettingsState& renderingDefaults) noexcept
	{
		ImGui::Spacing();
		ImGui::TextDisabled("Exposure");
		ImGui::TextWrapped("Unchecked properties use the current renderer defaults.");
		ViewportExposureOverrides exposureDraft = viewportSession.GetSettings().Exposure;
		if (ExposureSettingsEditor::DrawOverrides(exposureDraft, renderingDefaults))
		{
			viewportSession.SetExposureOverrides(exposureDraft);
		}
	}
}

void ViewportCameraProperties::OpenPopup() noexcept
{
	ImGui::OpenPopup("##ViewportCameraProperties");
}

void ViewportCameraProperties::BuildPopup(EditorViewportSession& viewportSession, const EngineRenderingSettingsState& renderingDefaults, bool bDisableInteraction) noexcept
{
	ImGui::SetNextWindowSizeConstraints(ImVec2(430.0f, 0.0f), ImVec2(560.0f, 720.0f));
	if (!ImGui::BeginPopup("##ViewportCameraProperties"))
	{
		return;
	}
	if (bDisableInteraction)
	{
		ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
		return;
	}

	DrawCameraProperties(viewportSession);
	DrawExposureOverrides(viewportSession, renderingDefaults);
	ImGui::EndPopup();
}
