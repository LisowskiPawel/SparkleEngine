#include "PCH.h"

#include "Gltf/GltfCameraImporter.h"

#include "Gltf/GltfCoordinateConverter.h"
#include "Core/Public/Diagnostics/Error.h"

#include <cgltf.h>

#include <DirectXMath.h>

#include <cmath>
#include <format>

class GltfCameraNaming final
{
public:
	static std::string ResolveCameraName(const cgltf_node& node)
	{
		if (node.name != nullptr && node.name[0] != '\0')
		{
			return node.name;
		}

		if (node.camera != nullptr && node.camera->name != nullptr && node.camera->name[0] != '\0')
		{
			return node.camera->name;
		}

		return {};
	}
};

void GltfCameraImporter::ImportCameras(const cgltf_data* data, SourceImportOutput& output)
{
	if (data == nullptr)
	{
		throw Diagnostics::Error("glTF camera import has no parsed scene.");
	}

	output.scene.cameras.reserve(data->cameras_count);
	for (cgltf_size nodeIndex = 0; nodeIndex < data->nodes_count; ++nodeIndex)
	{
		const cgltf_node& node = data->nodes[nodeIndex];
		if (node.camera == nullptr)
		{
			continue;
		}

		ImportedCamera camera;
		camera.name = GltfCameraNaming::ResolveCameraName(node);
		camera.sourceNodeIndex = static_cast<std::uint32_t>(nodeIndex);
		DirectX::XMStoreFloat4x4(&camera.worldTransform, GltfCoordinateConverter::ConvertCameraOrLightWorldTransform(GltfCoordinateConverter::ComputeNodeWorldTransform(&node)));

		const cgltf_camera& sourceCamera = *node.camera;
		if (sourceCamera.type != cgltf_camera_type_perspective)
		{
			throw Diagnostics::Error(std::format("glTF camera {} uses an unsupported projection.", nodeIndex));
		}

		const cgltf_camera_perspective& perspective = sourceCamera.data.perspective;
		// The renderer derives aspect ratio from the current viewport. A source aspect ratio is therefore intentionally
		// discarded, while the vertical field of view and finite clip range remain authoritative camera data.
		const bool bFarPlaneSpecified = perspective.has_zfar;
		const bool bProjectionFiniteyfov = bFarPlaneSpecified && std::isfinite(perspective.yfov);
		const bool bProjectionFiniteznear = bProjectionFiniteyfov && (std::isfinite(perspective.znear));
		const bool bProjectionFinite = bProjectionFiniteznear && (std::isfinite(perspective.zfar));
		const bool bProjectionOrdered0f = perspective.yfov <= 0.0f || perspective.znear <= 0.0f;
		const bool bProjectionOrderedznear = bProjectionOrdered0f || (perspective.zfar <= perspective.znear);
		const bool bProjectionOrderedznear2 = !bProjectionOrderedznear;
		const bool bProjectionOrdered = bProjectionFinite && bProjectionOrderedznear2;
		const bool bAspectRatioValidaspectratio = bProjectionOrdered && (perspective.has_aspect_ratio);
		const bool bAspectRatioValidaspectratio2 = bAspectRatioValidaspectratio && (!std::isfinite(perspective.aspect_ratio));
		const bool bAspectRatioValidGuard0f = bAspectRatioValidaspectratio && !bAspectRatioValidaspectratio2;
		const bool bAspectRatioValid0f = bAspectRatioValidGuard0f && (perspective.aspect_ratio <= 0.0f);
		const bool bAspectRatioValid0f2 = bAspectRatioValidaspectratio2 || bAspectRatioValid0f;
		const bool bAspectRatioValid0f3 = bAspectRatioValidaspectratio && bAspectRatioValid0f2;
		const bool bAspectRatioValid0f4 = bProjectionOrdered && !bAspectRatioValid0f3;
		const bool bAspectRatioValid = bProjectionOrdered && bAspectRatioValid0f4;

		if (!bAspectRatioValid)
		{
			throw Diagnostics::Error(std::format("glTF camera {} uses unsupported perspective parameters.", nodeIndex));
		}

		camera.projectionKind = ImportedCameraProjectionKind::Perspective;
		camera.fovYRadians = perspective.yfov;
		camera.nearZ = perspective.znear;
		camera.farZ = perspective.zfar;
		output.scene.cameras.push_back(std::move(camera));
	}
}
