#include "PCH.h"

#include "Gltf/GltfImporter.h"

#include "Gltf/GltfCameraImporter.h"
#include "Gltf/Animation/GltfAnimationImporter.h"
#include "Gltf/GltfScene.h"
#include "Gltf/Geometry/GltfGeometryImporter.h"
#include "Gltf/GltfLightImporter.h"
#include "Gltf/Materials/GltfMaterialImporter.h"
#include "Gltf/Materials/GltfMaterialVariantImporter.h"

#include <cgltf.h>

SourceImportOutput ImportGltfScene(const std::filesystem::path& filePath)
{
	SourceImportOutput output;
	output.provenance.sourceMetersPerUnit = 1.0f;
	const GltfScene scene(filePath);
	const cgltf_data& data = scene.GetData();

	GltfCameraImporter::ImportCameras(data, output.scene);
	GltfLightImporter::ImportLights(data, output.scene);

	const std::filesystem::path sourceDirectory = filePath.parent_path();
	output.scene.materials.reserve(data.materials_count);
	GltfMaterialImporter::ImportMaterials(data, sourceDirectory, output.scene);
	GltfMaterialVariantImporter::ImportMaterialVariants(data, output.scene);

	const std::size_t importedMeshInstanceCount = GltfGeometryImporter::CountImportedMeshInstances(data);
	output.scene.meshPrimitives.reserve(data.meshes_count);
	output.scene.meshInstances.reserve(importedMeshInstanceCount);
	output.scene.meshInstanceGroups.reserve(data.nodes_count);
	GltfGeometryImporter::ImportGeometry(data, output.scene);
	GltfAnimationImporter::ImportAnimations(data, output.scene);

	return output;
}
