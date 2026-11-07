#include "PCH.h"
#include "Scene/Inspection/SceneCameraInspector.h"

#include "Scene/SceneTransformEditor.h"
#include "Scene/Transactions/EditorTransactionHistory.h"
#include "Widgets/UiDetailsWidgets.h"
#include "World/WorldReadView.h"

#include <cstdio>

namespace SceneCameraInspector
{
	static void BuildTransformCategory(const std::string& filter, const WorldCameraReadData& camera, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept;
	static void BuildCameraCategory(const std::string& filter, const WorldCameraReadData& camera, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept;
	static void BuildAdvancedParametersCategory(const std::string& filter, const WorldCameraReadData& camera, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept;

	void Build(const WorldCameraReadData& camera, EditorTransactionHistory& transactionHistory, std::uint64_t generation, const std::string& filter) noexcept
	{
		BuildTransformCategory(filter, camera, transactionHistory, generation);
		BuildCameraCategory(filter, camera, transactionHistory, generation);
		BuildAdvancedParametersCategory(filter, camera, transactionHistory, generation);
	}

	static void BuildTransformCategory(const std::string& filter, const WorldCameraReadData& camera, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept
	{
		const bool bTransformMatches = EditorWidgets::MatchesDetailsFilter(filter, "Transform", "location rotation scale position");
		const bool bTransformOpen = bTransformMatches && EditorWidgets::DrawDetailsCategory("Transform");

		if (!bTransformOpen)
		{
			return;
		}

		Transform after = camera.LocalTransform;

		if (EditSceneTransform(after, {0.0f, 0.0f, -4.0f}))
		{
			(void) transactionHistory.Execute({0, SetLocalTransformCommand{camera.Entity, after}}, {0, SetLocalTransformCommand{camera.Entity, camera.LocalTransform}}, generation, "camera-transform");
		}
	}

	static void BuildCameraCategory(const std::string& filter, const WorldCameraReadData& camera, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept
	{
		const bool bCameraMatches = EditorWidgets::MatchesDetailsFilter(filter, "Camera", "field of view near clip far clip aspect ratio");
		const bool bCameraOpen = bCameraMatches && EditorWidgets::DrawDetailsCategory("Camera");

		if (!bCameraOpen)
		{
			return;
		}

		CameraDesc after = camera.Description;
		bool bChanged = false;
		const float defaultFov = 60.0f, defaultNear = 0.1f, defaultFar = 1000.0f;

		bChanged |= EditorWidgets::EditDetailsFloat("Field Of View", after.fovYDegrees, 0.1f, 1.0f, 179.0f, "%.1f", &defaultFov);
		bChanged |= EditorWidgets::EditDetailsFloat("Near Clip", after.nearZ, 0.01f, 0.001f, after.farZ, "%.3f", &defaultNear);
		bChanged |= EditorWidgets::EditDetailsFloat("Far Clip", after.farZ, 1.0f, after.nearZ, 100000.0f, "%.3f", &defaultFar);

		char buffer[64] = {};
		std::snprintf(buffer, sizeof(buffer), "%.3f", camera.AspectRatio);
		EditorWidgets::DrawDetailsValueRow("Aspect Ratio", buffer);

		if (bChanged)
		{
			(void) transactionHistory
			    .Execute({0, SetCameraDescriptionCommand{camera.Entity, after}}, {0, SetCameraDescriptionCommand{camera.Entity, camera.Description}}, generation, "camera-description");
		}
	}

	static void BuildAdvancedParametersCategory(const std::string& filter, const WorldCameraReadData& camera, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept
	{
		const bool bAdvancedMatches = EditorWidgets::MatchesDetailsFilter(filter, "Advanced", "visible visibility hidden");
		const bool bAdvancedOpen = bAdvancedMatches && EditorWidgets::DrawDetailsCategory("Advanced", false);

		if (!bAdvancedOpen)
		{
			return;
		}

		bool bVisible = camera.bVisible;
		const bool bDefaultVisible = true;
		if (EditorWidgets::EditDetailsCheckbox("Visible", bVisible, &bDefaultVisible))
		{
			(void)
			    transactionHistory.Execute({0, SetEntityVisibilityCommand{camera.Entity, bVisible}}, {0, SetEntityVisibilityCommand{camera.Entity, camera.bVisible}}, generation, "camera-visibility");
		}
	}
}
