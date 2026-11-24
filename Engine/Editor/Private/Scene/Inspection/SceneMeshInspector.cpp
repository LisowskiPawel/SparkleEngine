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
		const bool bTransformOpen = EditorWidgets::DrawDetailsCategory("Transform", filter, "location rotation scale transform");

		if (!bTransformOpen)
		{
			return;
		}
		Transform after = mesh.LocalTransform;

		if (EditSceneTransform(after, {0.0f, 0.0f, 0.0f}))
		{
			(void) transactionHistory.Execute({0, SetLocalTransformCommand{mesh.Entity, after}}, {0, SetLocalTransformCommand{mesh.Entity, mesh.LocalTransform}}, generation, "mesh-transform");
		}
	}

	static void BuildMeshCategory(const std::string& filter, const WorldMeshReadData& mesh) noexcept
	{
		const bool bMeshOpen = EditorWidgets::DrawDetailsCategory("Mesh", filter, "type asset rendering skeletal static");

		if (!bMeshOpen)
		{
			return;
		}

		const bool bSkeletal = mesh.Kind == SceneMeshKind::Skeletal;
		EditorWidgets::DrawDetailsValueRow("Type", bSkeletal ? "Skeletal" : "Static");
		char value[64] = {};
		std::snprintf(value, sizeof(value), "%llu", static_cast<unsigned long long>(mesh.MeshAssetId));
		EditorWidgets::DrawDetailsValueRow("Asset Id", value);
	}

	static void BuildAdvancedParametersCategory(const std::string& filter, const WorldMeshReadData& mesh, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept
	{
		const bool bAdvancedOpen = EditorWidgets::DrawDetailsCategory("Advanced", filter, "visible visibility hidden", false);

		if (!bAdvancedOpen)
		{
			return;
		}

		bool bVisible = mesh.bVisible;
		const bool bDefaultVisible = true;
		if (EditorWidgets::EditDetailsCheckbox("Visible", bVisible, &bDefaultVisible))
		{
			(void) transactionHistory.Execute({0, SetEntityVisibilityCommand{mesh.Entity, bVisible}}, {0, SetEntityVisibilityCommand{mesh.Entity, mesh.bVisible}}, generation, "mesh-visibility");
		}
	}

	static void BuildMaterialsCategory(const std::string& filter, const WorldMeshReadData& mesh) noexcept
	{
		const bool bMaterialsOpen = EditorWidgets::DrawDetailsCategory("Materials", filter, "element material slot surface");

		if (!bMaterialsOpen)
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
