#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

class EditorSceneSession;
struct SceneObjectSelection;

namespace SceneObjectPresentation
{
	struct Entry;
}

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
	std::size_t BuildSceneTable() noexcept;
	void BuildFooter(std::size_t displayedCount) noexcept;
	template <typename T> std::size_t DrawEntrySection(SceneOutlinerFilter filter, const char* id, const char* label, const char* emptyText, std::span<const T> entries, bool bDraw) noexcept;
	bool MatchesFilter(const SceneObjectPresentation::Entry& entry) const noexcept;
	bool DrawSectionRow(const char* id, const char* label, std::size_t count) noexcept;
	void DrawSelectionCell(const char* label, const SceneObjectSelection& selection, bool bIsSelected, int flags) noexcept;
	void DrawSelectionEntry(const SceneObjectPresentation::Entry& entry) noexcept;

	EditorSceneSession& m_scene;
	float m_widthPixels = 320.0f;
	float m_topInsetPixels = 0.0f;
	SceneOutlinerFilter m_activeFilter = SceneOutlinerFilter::All;
	std::string m_filterText;
};
