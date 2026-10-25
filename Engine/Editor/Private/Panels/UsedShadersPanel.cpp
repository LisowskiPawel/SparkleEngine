#include "PCH.h"

#include "Panels/UsedShadersPanel.h"

#include "Core/Public/Strings/StringUtils.h"
#include "Util/UiUtil.h"

#include <imgui.h>

#include <utility>

void UsedShadersPanel::SetGenerationProvider(RegisteredShaderListModel::GenerationProvider provider)
{
	m_model.SetGenerationProvider(std::move(provider));
	m_hasRows = false;
}

void UsedShadersPanel::SetReloadHandler(CommandHandler handler)
{
	m_reloadHandler = std::move(handler);
}

void UsedShadersPanel::SetRecookAllHandler(CommandHandler handler)
{
	m_recookAllHandler = std::move(handler);
}

void UsedShadersPanel::SetRecookHandler(RecookHandler handler)
{
	m_recookHandler = std::move(handler);
}

void UsedShadersPanel::BuildUI(bool disableInteraction)
{
	if (!m_isOpen)
	{
		return;
	}

	EnsureRows();
	ImGui::SetNextWindowSize(ImVec2(1180.0f, 760.0f), ImGuiCond_FirstUseEver);
	const std::string windowTitle = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Shader, "Shader Tools") + "##Shader Tools";
	if (!ImGui::Begin(windowTitle.c_str(), &m_isOpen))
	{
		ImGui::End();
		return;
	}

	DrawToolbar(disableInteraction);
	ImGui::Separator();
	const float availableHeight = ImGui::GetContentRegionAvail().y;
	const float tableHeight = availableHeight * 0.48f;
	ImGui::BeginChild("##UsedShadersTableRegion", ImVec2(0.0f, tableHeight), ImGuiChildFlags_None);
	DrawTable(disableInteraction);
	ImGui::EndChild();
	const std::string inspectionLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Shader, "Selected Shader Inspection");
	ImGui::SeparatorText(inspectionLabel.c_str());
	DrawSelectedShaderArtifacts();
	ImGui::End();
}

void UsedShadersPanel::DrawTextArtifact(const char* childId, const std::string& text) noexcept
{
	ImGui::BeginChild(childId, ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::InputTextMultiline(
	    "##ArtifactText",
	    const_cast<char*>(text.c_str()),
	    text.size() + 1,
	    ImGui::GetContentRegionAvail(),
	    ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_AllowTabInput);

	ImGui::EndChild();
}

void UsedShadersPanel::EnsureRows()
{
	if (m_hasRows)
	{
		return;
	}

	m_model.Refresh();
	m_hasRows = true;
}

void UsedShadersPanel::DrawToolbar(bool disableInteraction)
{
	ImGui::BeginDisabled(disableInteraction);
	const std::string refreshLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Refresh, "Refresh");
	if (ImGui::Button(refreshLabel.c_str()))
	{
		m_model.Refresh();
		m_hasRows = true;
		m_artifactInspection.Invalidate();
		m_artifactInspection.Select(GetSelectedRow());
	}
	ImGui::SameLine();
	const std::string reloadLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Reload, "Reload Cooked");
	if (ImGui::Button(reloadLabel.c_str()) && m_reloadHandler)
	{
		m_reloadHandler();
	}
	ImGui::SameLine();
	const std::string recookAllLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Refresh, "Recook All");
	if (ImGui::Button(recookAllLabel.c_str()) && m_recookAllHandler)
	{
		m_recookAllHandler();
	}
	ImGui::SameLine();

	const RegisteredShaderRow* selectedRow = GetSelectedRow();
	const std::string recookSelectedLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Refresh, "Recook Selected");
	if (ImGui::Button(recookSelectedLabel.c_str()) && selectedRow != nullptr && m_recookHandler)
	{
		m_recookHandler(selectedRow->ShaderId);
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(260.0f);
	const std::string filterHint = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Search, "Filter shader/source");
	ImGui::InputTextWithHint("##UsedShadersFilter", filterHint.c_str(), m_filterBuffer.data(), m_filterBuffer.size());
	ImGui::SameLine();
	ImGui::TextDisabled("%zu registered shader stage(s)", m_model.GetRows().size());
	ImGui::EndDisabled();
}

void UsedShadersPanel::DrawTable(bool disableInteraction)
{
	const ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
	if (!ImGui::BeginTable("##UsedShadersTableV2", 9, tableFlags, ImVec2(0.0f, 0.0f)))
	{
		return;
	}

	ImGui::TableSetupScrollFreeze(0, 1);
	ImGui::TableSetupColumn("Shader Id");
	ImGui::TableSetupColumn("Shader Type Id");
	ImGui::TableSetupColumn("Stage");
	ImGui::TableSetupColumn("Source Path");
	ImGui::TableSetupColumn("Entry");
	ImGui::TableSetupColumn("Params");
	ImGui::TableSetupColumn("Backend/Target");
	ImGui::TableSetupColumn("Generation");
	ImGui::TableSetupColumn("Artifacts");
	ImGui::TableHeadersRow();

	ImGui::BeginDisabled(disableInteraction);
	for (const RegisteredShaderRow& row : m_model.GetRows())
	{
		if (!MatchesFilter(row))
		{
			continue;
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		const bool selected = row.ShaderId == m_selectedShaderId;
		if (ImGui::Selectable(row.ShaderId.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns))
		{
			m_selectedShaderId = row.ShaderId;
			m_artifactInspection.Select(GetSelectedRow());
		}
		ImGui::TableNextColumn();
		ImGui::Text("%016llx", static_cast<unsigned long long>(row.ShaderTypeId));
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(row.Stage.c_str());
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(row.SourcePath.c_str());
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(row.EntryPoint.c_str());
		ImGui::TableNextColumn();
		ImGui::Text("%zu", row.ParameterCount);
		ImGui::TableNextColumn();
		ImGui::TextUnformatted("auto / DxilSm66");
		ImGui::TableNextColumn();
		ImGui::Text("%llu", static_cast<unsigned long long>(row.RuntimeGeneration));
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(row.ArtifactDirectory.empty() ? "missing" : "available");
	}
	ImGui::EndDisabled();
	ImGui::EndTable();
}

void UsedShadersPanel::DrawSelectedShaderArtifacts()
{
	const RegisteredShaderRow* selectedRow = GetSelectedRow();
	if (selectedRow == nullptr)
	{
		ImGui::TextDisabled("Select a shader row to inspect source, reflection, disassembly, parameter match, and compile request artifacts.");
		return;
	}

	m_artifactInspection.Select(GetSelectedRow());
	ImGui::TextUnformatted(selectedRow->ShaderId.c_str());
	ImGui::SameLine();
	ImGui::TextDisabled("%016llx | %s | %s", static_cast<unsigned long long>(selectedRow->ShaderTypeId), selectedRow->Stage.c_str(), selectedRow->EntryPoint.c_str());
	if (m_artifactInspection.GetDirectory().empty())
	{
		ImGui::TextWrapped(
		    "No debug artifact bundle is available for this shader. Recook shaders with debug artifacts enabled to populate inspection "
		    "data.");

		return;
	}

	ImGui::TextDisabled("%s", m_artifactInspection.GetDirectory().generic_string().c_str());
	if (ImGui::BeginTabBar("##SelectedShaderArtifactTabs"))
	{
		const std::string sourceLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::SourceFile, "Source");
		if (ImGui::BeginTabItem(sourceLabel.c_str()))
		{
			DrawTextArtifact("##SelectedShaderSourceText", m_artifactInspection.GetTexts().Source);
			ImGui::EndTabItem();
		}
		const std::string reflectionLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Reflection, "Reflection");
		if (ImGui::BeginTabItem(reflectionLabel.c_str()))
		{
			DrawTextArtifact("##SelectedShaderReflectionText", m_artifactInspection.GetTexts().Reflection);
			ImGui::EndTabItem();
		}
		const std::string disassemblyLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Disassembly, "Disassembly");
		if (ImGui::BeginTabItem(disassemblyLabel.c_str()))
		{
			DrawTextArtifact("##SelectedShaderDisassemblyText", m_artifactInspection.GetTexts().Disassembly);
			ImGui::EndTabItem();
		}
		const std::string paramMatchLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Filter, "Param Match");
		if (ImGui::BeginTabItem(paramMatchLabel.c_str()))
		{
			DrawTextArtifact("##SelectedShaderParamMatchText", m_artifactInspection.GetTexts().ParameterMatch);
			ImGui::EndTabItem();
		}
		const std::string compileRequestLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::CompileRequest, "Compile Request");
		if (ImGui::BeginTabItem(compileRequestLabel.c_str()))
		{
			DrawTextArtifact("##SelectedShaderCompileRequestText", m_artifactInspection.GetTexts().CompileRequest);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

const RegisteredShaderRow* UsedShadersPanel::GetSelectedRow() const noexcept
{
	for (const RegisteredShaderRow& row : m_model.GetRows())
	{
		if (row.ShaderId == m_selectedShaderId)
		{
			return &row;
		}
	}
	return nullptr;
}

bool UsedShadersPanel::MatchesFilter(const RegisteredShaderRow& row) const noexcept
{
	const std::string_view filter(m_filterBuffer.data());
	return filter.empty() || Strings::ContainsIgnoreCase(row.ShaderId, filter) || Strings::ContainsIgnoreCase(row.SourcePath, filter) || Strings::ContainsIgnoreCase(row.Stage, filter);
}
