#include "PCH.h"

#include "Panels/ShaderToolsPanel.h"

#include "Core/Public/Strings/StringUtils.h"
#include "Util/UiUtil.h"

#include <imgui.h>

#include <utility>

void ShaderToolsPanel::SetDiagnosticsProvider(RegisteredShaderListModel::DiagnosticsProvider provider)
{
	m_model.SetDiagnosticsProvider(std::move(provider));
	m_hasRows = false;
}

void ShaderToolsPanel::SetReloadHandler(CommandHandler handler)
{
	m_reloadHandler = std::move(handler);
}

void ShaderToolsPanel::SetCompileHandler(std::function<ConsoleCommandResult(bool)> handler)
{
	m_compileHandler = std::move(handler);
}

void ShaderToolsPanel::SetCompileStatus(bool busy, std::string_view message)
{
	m_compiling = busy;
	if (!message.empty())
	{
		m_compileStatus = message.substr(0, message.find('\n'));
		m_hasRows = false;
		m_exportInspection.Invalidate();
	}
}

ConsoleCommandResult ShaderToolsPanel::RequestCompile(bool generateSymbols)
{
	ConsoleCommandResult result = m_compileHandler ? m_compileHandler(generateSymbols) : ConsoleCommandResult::Error("Shader compilation is unavailable in this host.");
	m_compileStatus = result.Message;
	return result;
}

void ShaderToolsPanel::BuildUI(bool disableInteraction)
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
	ImGui::BeginChild("##ShaderToolsTableRegion", ImVec2(0.0f, tableHeight), ImGuiChildFlags_None);
	DrawTable(disableInteraction);
	ImGui::EndChild();
	const std::string inspectionLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Shader, "Selected Shader Inspection");
	ImGui::SeparatorText(inspectionLabel.c_str());
	DrawSelectedShaderExport();
	ImGui::End();
}

void ShaderToolsPanel::DrawExportText(const char* childId, const std::string& text) noexcept
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

void ShaderToolsPanel::EnsureRows()
{
	if (m_hasRows)
	{
		return;
	}

	m_model.Refresh();
	m_hasRows = true;
}

void ShaderToolsPanel::DrawToolbar(bool disableInteraction)
{
	ImGui::BeginDisabled(disableInteraction);
	ImGui::BeginDisabled(m_compiling || !m_compileHandler);
	if (ImGui::Button(m_compiling ? "Compiling shaders..." : "Recompile Shaders"))
	{
		(void) RequestCompile(m_generateSymbols);
	}
	ImGui::SameLine();
	ImGui::Checkbox("Generate shader symbols", &m_generateSymbols);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Export private shader debug data; runtime optimization is unchanged.");
	}
	ImGui::EndDisabled();
	ImGui::SameLine();
	const std::string refreshLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Refresh, "Refresh");
	if (ImGui::Button(refreshLabel.c_str()))
	{
		m_model.Refresh();
		m_hasRows = true;
		m_exportInspection.Invalidate();
	}
	ImGui::SameLine();
	const std::string reloadLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Reload, "Reload Cooked");
	if (ImGui::Button(reloadLabel.c_str()) && m_reloadHandler)
	{
		m_reloadHandler();
		m_hasRows = false;
		m_exportInspection.Invalidate();
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(260.0f);
	const std::string filterHint = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Search, "Filter shader/source");
	ImGui::InputTextWithHint("##ShaderToolsFilter", filterHint.c_str(), m_filterBuffer.data(), m_filterBuffer.size());
	ImGui::SameLine();
	ImGui::TextDisabled("%zu registered shader stage(s)", m_model.GetRows().size());
	ImGui::EndDisabled();
	if (!m_compileStatus.empty())
	{
		ImGui::TextWrapped("%s", m_compileStatus.c_str());
	}
}

void ShaderToolsPanel::DrawTable(bool disableInteraction)
{
	const ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
	if (!ImGui::BeginTable("##RegisteredShadersTable", 8, tableFlags, ImVec2(0.0f, 0.0f)))
	{
		return;
	}

	ImGui::TableSetupScrollFreeze(0, 1);
	ImGui::TableSetupColumn("Shader");
	ImGui::TableSetupColumn("Shader Type Id");
	ImGui::TableSetupColumn("Stage");
	ImGui::TableSetupColumn("Source Path");
	ImGui::TableSetupColumn("Entry");
	ImGui::TableSetupColumn("Params");
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
		const bool selected = row.ShaderTypeId == m_selectedShaderTypeId;
		if (ImGui::Selectable(row.ShaderName.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns))
		{
			m_selectedShaderTypeId = row.ShaderTypeId;
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
		ImGui::Text("%llu", static_cast<unsigned long long>(row.RuntimeGeneration));
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(row.ExportDirectory.empty() ? "missing" : "available");
	}
	ImGui::EndDisabled();
	ImGui::EndTable();
}

void ShaderToolsPanel::DrawSelectedShaderExport()
{
	const RegisteredShaderRow* selectedRow = GetSelectedRow();
	m_exportInspection.InspectExport(selectedRow != nullptr ? selectedRow->ExportDirectory : std::filesystem::path{});
	if (selectedRow == nullptr)
	{
		ImGui::TextDisabled("Select a shader row to inspect source, reflection, disassembly, parameter match, and compile request artifacts.");
		return;
	}

	ImGui::TextUnformatted(selectedRow->ShaderName.c_str());
	ImGui::SameLine();
	ImGui::TextDisabled("%016llx | %s | %s", static_cast<unsigned long long>(selectedRow->ShaderTypeId), selectedRow->Stage.c_str(), selectedRow->EntryPoint.c_str());
	if (!m_exportInspection.HasExport())
	{
		ImGui::TextWrapped(
		    "No shader export is available for this shader. Enable Generate shader symbols and compile shaders to populate inspection "
		    "data.");

		return;
	}

	ImGui::TextDisabled("%s", m_exportInspection.GetDirectory().generic_string().c_str());
	if (ImGui::BeginTabBar("##SelectedShaderExportTabs"))
	{
		const std::string sourceLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::SourceFile, "Source");
		if (ImGui::BeginTabItem(sourceLabel.c_str()))
		{
			DrawExportText("##SelectedShaderSourceText", m_exportInspection.GetTexts().Source);
			ImGui::EndTabItem();
		}
		const std::string reflectionLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Reflection, "Reflection");
		if (ImGui::BeginTabItem(reflectionLabel.c_str()))
		{
			DrawExportText("##SelectedShaderReflectionText", m_exportInspection.GetTexts().Reflection);
			ImGui::EndTabItem();
		}
		const std::string disassemblyLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Disassembly, "Disassembly");
		if (ImGui::BeginTabItem(disassemblyLabel.c_str()))
		{
			DrawExportText("##SelectedShaderDisassemblyText", m_exportInspection.GetTexts().Disassembly);
			ImGui::EndTabItem();
		}
		const std::string paramMatchLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Filter, "Param Match");
		if (ImGui::BeginTabItem(paramMatchLabel.c_str()))
		{
			DrawExportText("##SelectedShaderParamMatchText", m_exportInspection.GetTexts().ParameterMatch);
			ImGui::EndTabItem();
		}
		const std::string compileRequestLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::CompileRequest, "Compile Request");
		if (ImGui::BeginTabItem(compileRequestLabel.c_str()))
		{
			DrawExportText("##SelectedShaderCompileRequestText", m_exportInspection.GetTexts().CompileRequest);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

const RegisteredShaderRow* ShaderToolsPanel::GetSelectedRow() const noexcept
{
	for (const RegisteredShaderRow& row : m_model.GetRows())
	{
		if (row.ShaderTypeId == m_selectedShaderTypeId)
		{
			return &row;
		}
	}
	return nullptr;
}

bool ShaderToolsPanel::MatchesFilter(const RegisteredShaderRow& row) const noexcept
{
	const std::string_view filter(m_filterBuffer.data());
	return filter.empty() || Strings::ContainsIgnoreCase(row.ShaderName, filter) || Strings::ContainsIgnoreCase(row.SourcePath, filter) || Strings::ContainsIgnoreCase(row.Stage, filter);
}
