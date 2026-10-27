#include "PCH.h"
#include "Panels/SceneInspectorPanel.h"

#include "Panels/SceneCameraInspector.h"
#include "Panels/SceneLightInspector.h"
#include "Panels/SceneMaterialVariantInspector.h"
#include "Panels/SceneMeshInspector.h"
#include "Panels/SceneSkyInspector.h"
#include "Scene/SceneObjectSelection.h"
#include "Scene/SceneObjectPresentation.h"
#include "Scene/Model/EditorSceneModel.h"
#include "Scene/Transactions/EditorTransactionHistory.h"
#include "Style/SparkleUiPalette.h"
#include "Util/UiUtil.h"

#include <algorithm>
#include <string>
#include <optional>

#include <imgui.h>

SceneInspectorPanel::SceneInspectorPanel(SceneObjectSelection& selection, EditorTransactionHistory& transactionHistory, float widthPixels) noexcept :
    m_transactionHistory(&transactionHistory),
    m_selection(&selection),
    m_widthPixels(widthPixels)
{
}

void SceneInspectorPanel::BuildDetailsToolbar() noexcept
{
	char filterBuffer[128] = {};
	const std::size_t copyLength = (std::min) (m_filterText.size(), sizeof(filterBuffer) - 1);
	if (copyLength > 0)
	{
		std::copy_n(m_filterText.data(), copyLength, filterBuffer);
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 5.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.055f, 0.058f, 0.064f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.075f, 0.080f, 0.090f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.070f, 0.095f, 0.130f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, UiUtil::WithAlphaU32(SparkleUiPalette::Border(), 0.70f));
	ImGui::SetNextItemWidth(-1.0f);
	const std::string searchHint = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Search, "Search");
	if (ImGui::InputTextWithHint("##DetailsFilter", searchHint.c_str(), filterBuffer, sizeof(filterBuffer)))
	{
		m_filterText = filterBuffer;
	}
	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar(3);
}

void SceneInspectorPanel::SetWidth(float widthPixels) noexcept
{
	m_widthPixels = widthPixels;
}

void SceneInspectorPanel::SetTopInset(float topInsetPixels) noexcept
{
	m_topInsetPixels = topInsetPixels;
}

void SceneInspectorPanel::BuildSelectionInspector() noexcept
{
	SceneObjectPresentation::DrawSelectionHeader(m_model.get(), m_selection);
	BuildDetailsToolbar();
	switch (m_selection->type)
	{
		case SceneObjectType::Camera:
			if (const WorldCameraReadData* camera = m_model->FindCamera(m_selection->entity))
			{
				SceneCameraInspector::Build(*camera, *m_transactionHistory, m_model->GetWorldGeneration(), m_filterText);
			}
			break;
		case SceneObjectType::Light:
			if (const WorldLightReadData* light = m_model->FindLight(m_selection->entity))
			{
				SceneLightInspector::Build(light->Description, light->Entity, *m_transactionHistory, m_model->GetWorldGeneration(), m_filterText);
			}
			break;
		case SceneObjectType::Sky:
			SceneSkyInspector::Build(m_model->GetSkyEnvironment(), *m_transactionHistory, m_model->GetWorldGeneration(), m_filterText);
			break;
		case SceneObjectType::Mesh:
			if (const WorldMeshReadData* mesh = m_model->FindMesh(m_selection->entity))
			{
				SceneMeshInspector::Build(*mesh, *m_transactionHistory, m_model->GetWorldGeneration(), m_filterText);
			}
			break;
		case SceneObjectType::None:
		default:
			UiUtil::DrawDetailsEmptyState();
			break;
	}
}

void SceneInspectorPanel::BuildEditStatus() noexcept
{
	const std::optional<WorldEditResult>& result = m_transactionHistory->GetLastResult();
	if (!result || result->IsAccepted())
	{
		return;
	}

	ImGui::TextWrapped("Edit not applied: %s", result->Message.c_str());
	if (ImGui::SmallButton("Dismiss edit failure"))
	{
		m_transactionHistory->DismissLastResult();
	}
	ImGui::Separator();
}

void SceneInspectorPanel::BeginWindow() noexcept
{
	ImGuiIO& io = ImGui::GetIO();

	constexpr float kMinWidth = 320.0f;
	const float kMaxWidth = (std::max) (kMinWidth + 1.0f, io.DisplaySize.x * 0.7f);
	m_widthPixels = std::clamp(m_widthPixels, kMinWidth, kMaxWidth);
	const float panelHeight = (std::max) (1.0f, io.DisplaySize.y - m_topInsetPixels);

	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - m_widthPixels, m_topInsetPixels), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(m_widthPixels, panelHeight), ImGuiCond_Once);
	ImGui::SetNextWindowSizeConstraints(ImVec2(kMinWidth, panelHeight), ImVec2(kMaxWidth, panelHeight));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
	ImGui::PopStyleVar();

	m_widthPixels = ImGui::GetWindowWidth();
}

void SceneInspectorPanel::BuildInspectorTabs() noexcept
{
	constexpr ImGuiTabBarFlags kTabBarFlags = ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("##InspectorTabs", kTabBarFlags))
	{
		if (ImGui::BeginTabItem("Details"))
		{
			ImGui::Spacing();
			BuildSelectionInspector();
			ImGui::EndTabItem();
		}

		if (!m_model->GetMaterialVariants().Names.empty() && ImGui::BeginTabItem("Variants"))
		{
			ImGui::Spacing();
			SceneMaterialVariantInspector::Build(m_model->GetMaterialVariants(), *m_transactionHistory, m_model->GetWorldGeneration());
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void SceneInspectorPanel::BuildUI(bool disableInteraction)
{
	BeginWindow();

	if (!m_model || !m_transactionHistory || m_selection == nullptr)
	{
		ImGui::TextDisabled("Scene inspector unavailable");
		ImGui::End();
		return;
	}

	ImGui::BeginDisabled(disableInteraction);

	constexpr float kContentPad = 8.0f;
	ImGui::Indent(kContentPad);
	BuildEditStatus();
	BuildInspectorTabs();

	ImGui::Unindent(kContentPad);

	ImGui::EndDisabled();

	ImGui::End();
}
