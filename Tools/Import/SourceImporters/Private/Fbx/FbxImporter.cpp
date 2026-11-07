#include "PCH.h"

#include "Fbx/FbxImporter.h"

#include "Fbx/FbxAnimationImporter.h"
#include "Fbx/FbxCameraImporter.h"
#include "Fbx/FbxEmbeddedTextureImporter.h"
#include "Assimp/AssimpGeometryImporter.h"
#include "Fbx/FbxLightImporter.h"
#include "Assimp/AssimpMaterialImporter.h"
#include "Assimp/AssimpSceneReader.h"
#include "Core/Public/Diagnostics/Error.h"

#include <assimp/Importer.hpp>

#include <format>
#include <vector>

SourceImportOutput ImportFbxScene(const std::filesystem::path& filePath)
{
	SourceImportOutput output;
	output.provenance.sourcePath = filePath;

	Assimp::Importer importer;
	const aiScene& scene = AssimpSceneReader::LoadScene(filePath, importer);
	const float sourceMetersPerUnit = AssimpSceneReader::GetFbxMetersPerSourceUnit(importer);
	output.provenance.sourceMetersPerUnit = sourceMetersPerUnit;
	output.scene.materials.reserve(scene.mNumMaterials);
	const std::size_t importedMeshInstanceCount = AssimpGeometryImporter::CountImportedMeshInstances(*scene.mRootNode);
	output.ReserveMeshPrimitives(scene.mNumMeshes);
	output.ReserveMeshInstances(importedMeshInstanceCount);

	const std::vector<std::filesystem::path> embeddedTexturePaths = FbxEmbeddedTextureImporter::ExtractTextures(scene);
	AssimpMaterialImporter::ImportMaterials(scene, filePath.parent_path(), embeddedTexturePaths, output);

	AssimpGeometryImporter::ImportGeometry(scene, output);
	FbxCameraImporter::ImportCameras(scene, sourceMetersPerUnit, output);
	FbxLightImporter::ImportLights(scene, sourceMetersPerUnit, output);
	FbxAnimationImporter::ImportAnimations(scene, output);

	const bool bOrInconsistentMeshInstancesEmpty = output.scene.meshPrimitives.empty() != output.scene.meshInstances.empty();
	const bool bOrInconsistentMeshPrimitivesEmpty = !bOrInconsistentMeshInstancesEmpty && (output.scene.meshPrimitives.empty());
	const bool bOrInconsistentCamerasEmpty = bOrInconsistentMeshPrimitivesEmpty && (output.scene.cameras.empty());
	const bool bImportedOrInconsistentLightsEmpty = bOrInconsistentCamerasEmpty && (output.scene.lights.empty());
	const bool bOrInconsistentAnimationsEmpty = bImportedOrInconsistentLightsEmpty && (output.scene.animations.empty());
	const bool bInvalidScene = bOrInconsistentMeshInstancesEmpty || bOrInconsistentAnimationsEmpty;

	if (bInvalidScene)
	{
		throw Diagnostics::Error("FBX import produced incomplete mesh content or no supported scene content.");
	}

	return output;
}
