#include "PCH.h"

#include "Gltf/GltfCameraImporter.h"

#include "Gltf/GltfCoordinateConverter.h"
#include "Core/Public/Diagnostics/Error.h"

#include <cgltf.h>

#include <DirectXMath.h>

#include <cmath>
#include <format>

namespace GltfCameraImporter
{
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

	void ImportCameras(const cgltf_data& data, ImportedScene& importedScene)
	{
		importedScene.cameras.reserve(data.cameras_count);
		for (cgltf_size nodeIndex = 0; nodeIndex < data.nodes_count; ++nodeIndex)
		{
			const cgltf_node& node = data.nodes[nodeIndex];
			if (node.camera == nullptr)
			{
				continue;
			}

			ImportedCamera camera;
			camera.name = ResolveCameraName(node);
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
			const bool bFovFinite = std::isfinite(perspective.yfov);
			const bool bClipPlanesFinite = std::isfinite(perspective.znear) && std::isfinite(perspective.zfar);
			const bool bProjectionFinite = bFovFinite && bClipPlanesFinite;
			const bool bPositiveNearRange = perspective.yfov > 0.0f && perspective.znear > 0.0f;
			const bool bClipPlanesOrdered = bPositiveNearRange && perspective.zfar > perspective.znear;
			if (!perspective.has_zfar || !bProjectionFinite || !bClipPlanesOrdered)
			{
				throw Diagnostics::Error(std::format("glTF camera {} uses unsupported perspective parameters.", nodeIndex));
			}
			if (perspective.has_aspect_ratio)
			{
				const bool bAspectValid = std::isfinite(perspective.aspect_ratio) && perspective.aspect_ratio > 0.0f;
				if (!bAspectValid)
				{
					throw Diagnostics::Error(std::format("glTF camera {} uses unsupported perspective parameters.", nodeIndex));
				}
			}

			camera.projectionKind = ImportedCameraProjectionKind::Perspective;
			camera.fovYRadians = perspective.yfov;
			camera.nearZ = perspective.znear;
			camera.farZ = perspective.zfar;
			importedScene.cameras.push_back(std::move(camera));
		}
	}
}
