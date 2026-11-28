#include "PCH.h"
#include "Scene/Inspection/SceneMeshInspector.h"

#include "Scene/SceneTransformEditor.h"
#include "Scene/Transactions/EditorTransactionHistory.h"
#include "Widgets/UiDetailsWidgets.h"
#include "World/WorldReadView.h"

#include <cstdio>

namespace SceneMeshInspector
{
	static void BuildTransformCategory(const WorldMeshReadData& mesh, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept;
	static void BuildMeshCategory(const WorldMeshReadData& mesh) noexcept;
	static void BuildAdvancedParametersCategory(const WorldMeshReadData& mesh, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept;
	static void BuildMaterialsCategory(const WorldMeshReadData& mesh) noexcept;

	void Build(const WorldMeshReadData& mesh, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept
	{
		BuildTransformCategory(mesh, transactionHistory, generation);
		BuildMeshCategory(mesh);
		BuildAdvancedParametersCategory(mesh, transactionHistory, generation);
		BuildMaterialsCategory(mesh);
	}

	static void BuildTransformCategory(const WorldMeshReadData& mesh, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept
	{
		const bool bTransformOpen = EditorWidgets::DrawDetailsCategory("Transform");

		if (!bTransformOpen)
		{
			return;
		}
		Transform after = mesh.LocalTransform;

		if (EditSceneTransform(after, {0.0f, 0.0f, 0.0f}))
		{
			(void) transactionHistory.Execute(SetLocalTransformCommand{mesh.Entity, after}, SetLocalTransformCommand{mesh.Entity, mesh.LocalTransform}, generation, "mesh-transform");
		}
	}

	static void BuildMeshCategory(const WorldMeshReadData& mesh) noexcept
	{
		const bool bMeshOpen = EditorWidgets::DrawDetailsCategory("Mesh");

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

	static void BuildAdvancedParametersCategory(const WorldMeshReadData& mesh, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept
	{
		const bool bAdvancedOpen = EditorWidgets::DrawDetailsCategory("Advanced", false);

		if (!bAdvancedOpen)
		{
			return;
		}

		bool bVisible = mesh.bVisible;
		const bool bDefaultVisible = true;
		if (EditorWidgets::EditDetailsCheckbox("Visible", bVisible, &bDefaultVisible))
		{
			(void) transactionHistory.Execute(SetEntityVisibilityCommand{mesh.Entity, bVisible}, SetEntityVisibilityCommand{mesh.Entity, mesh.bVisible}, generation, "mesh-visibility");
		}
	}

	static void BuildMaterialsCategory(const WorldMeshReadData& mesh) noexcept
	{
		const bool bMaterialsOpen = EditorWidgets::DrawDetailsCategory("Materials");

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
