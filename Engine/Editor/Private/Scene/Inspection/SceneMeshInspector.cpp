#include "PCH.h"
#include "Scene/Inspection/SceneMeshInspector.h"

#include "Scene/SceneTransformEditor.h"
#include "Scene/Transactions/EditorTransactionHistory.h"
#include "Widgets/UiDetailsWidgets.h"
#include "World/WorldReadView.h"

#include <cstdio>

namespace SceneMeshInspector
{
	static void BuildTransformCategory(const std::string& filter, const WorldMeshReadData& mesh, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept;
	static void BuildMeshCategory(const std::string& filter, const WorldMeshReadData& mesh) noexcept;
	static void BuildAdvancedParametersCategory(const std::string& filter, const WorldMeshReadData& mesh, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept;
	static void BuildMaterialsCategory(const std::string& filter, const WorldMeshReadData& mesh) noexcept;

	void Build(const WorldMeshReadData& mesh, EditorTransactionHistory& transactionHistory, std::uint64_t generation, const std::string& filter) noexcept
	{
		BuildTransformCategory(filter, mesh, transactionHistory, generation);
		BuildMeshCategory(filter, mesh);
		BuildAdvancedParametersCategory(filter, mesh, transactionHistory, generation);
		BuildMaterialsCategory(filter, mesh);
	}

	static void BuildTransformCategory(const std::string& filter, const WorldMeshReadData& mesh, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept
	{
		const bool transformMatches = EditorWidgets::MatchesDetailsFilter(filter, "Transform", "location rotation scale transform");
		const bool transformOpen = transformMatches && EditorWidgets::DrawDetailsCategory("Transform");
		
		if (!transformOpen)
		{
			return;
		}
		Transform after = mesh.LocalTransform;

		if (SceneTransformEditor::Edit(after, {0.0f, 0.0f, 0.0f}))
		{
			(void) transactionHistory.Execute({0, SetLocalTransformCommand{mesh.Entity, after}}, {0, SetLocalTransformCommand{mesh.Entity, mesh.LocalTransform}}, generation, "mesh-transform");
		}
	}

	static void BuildMeshCategory(const std::string& filter, const WorldMeshReadData& mesh) noexcept
	{
		const bool meshMatches = EditorWidgets::MatchesDetailsFilter(filter, "Mesh", "type asset rendering skeletal static");
		const bool meshOpen = meshMatches && EditorWidgets::DrawDetailsCategory("Mesh");
		
		if (!meshOpen)
		{
			return;
		}
		const bool skeletal = mesh.Kind == SceneMeshKind::Skeletal;
		EditorWidgets::DrawDetailsValueRow("Type", skeletal ? "Skeletal" : "Static");
		char value[64] = {};
		std::snprintf(value, sizeof(value), "%llu", static_cast<unsigned long long>(mesh.MeshAssetId));
		EditorWidgets::DrawDetailsValueRow("Asset Id", value);
	}

	static void BuildAdvancedParametersCategory(const std::string& filter, const WorldMeshReadData& mesh, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept
	{
		const bool advancedMatches = EditorWidgets::MatchesDetailsFilter(filter, "Advanced", "visible visibility hidden");
		const bool advancedOpen = advancedMatches && EditorWidgets::DrawDetailsCategory("Advanced", false);
		
		if (!advancedOpen)
		{
			return;
		}
		bool visible = mesh.Visible;
		const bool defaultVisible = true;
		if (EditorWidgets::EditDetailsCheckbox("Visible", visible, &defaultVisible))
		{
			(void) transactionHistory.Execute({0, SetEntityVisibilityCommand{mesh.Entity, visible}}, {0, SetEntityVisibilityCommand{mesh.Entity, mesh.Visible}}, generation, "mesh-visibility");
		}
	}

	static void BuildMaterialsCategory(const std::string& filter, const WorldMeshReadData& mesh) noexcept
	{
		const bool materialsMatches = EditorWidgets::MatchesDetailsFilter(filter, "Materials", "element material slot surface");
		const bool materialsOpen = materialsMatches && EditorWidgets::DrawDetailsCategory("Materials");
		
		if (!materialsOpen)
		{
			return;
		}
		char buffer[64] = "None";
		if (mesh.Material.IsValid())
		{
			std::snprintf(buffer, sizeof(buffer), "Material %u (generation %u)", mesh.Material.GetIndex(), mesh.Material.GetGeneration());
		}
		EditorWidgets::DrawDetailsValueRow("Element 0", buffer);
	}
}
