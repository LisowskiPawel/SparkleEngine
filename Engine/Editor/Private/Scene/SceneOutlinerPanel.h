#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

class EditorSceneSession;
struct EditorSceneEntry;
struct SceneObjectSelection;

enum class SceneOutlinerFilter : std::uint8_t
{
	All,
	Cameras,
	Lights,
	Meshes
};

class SceneOutlinerPanel final
{
public:
	explicit SceneOutlinerPanel(EditorSceneSession& scene, float widthPixels = 320.0f) noexcept;

	float GetWidth() const noexcept { return m_widthPixels; }

	void SetTopInset(float topInsetPixels) noexcept;

	void BuildUI(bool bDisableInteraction = false);

private:
	void BeginWindow() noexcept;
	void BuildToolbar() noexcept;
	void BuildSceneTable() noexcept;
	void BuildFooter() noexcept;
	void DrawEntrySection(SceneOutlinerFilter filter, const char* id, const char* label, const char* emptyText, std::span<const EditorSceneEntry> entries) noexcept;
	bool PassesActiveFilter(const SceneObjectSelection& selection) const noexcept;
	bool MatchesFilter(const EditorSceneEntry& entry) const noexcept;
	std::size_t CountVisibleEntries() const noexcept;
	bool IsEntryVisible(const SceneObjectSelection& selection) const noexcept;
	void DrawSectionRow(const char* id, const char* label, std::size_t count, bool& bOpen) noexcept;
	void DrawSelectionCell(const char* label, const SceneObjectSelection& selection, bool bIsSelected, int flags) noexcept;
	void DrawSelectionEntry(const char* label, const char* typeLabel, const SceneObjectSelection& selection) noexcept;

	EditorSceneSession& m_scene;
	float m_widthPixels = 320.0f;
	float m_topInsetPixels = 0.0f;
	SceneOutlinerFilter m_activeFilter = SceneOutlinerFilter::All;
	std::string m_filterText;
};
