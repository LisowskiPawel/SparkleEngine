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
#include <assimp/config.h>

#include <cmath>
#include <vector>

static float ReadSourceMetersPerUnit(const Assimp::Importer& importer)
{
	const float metersPerSourceUnit = importer.GetPropertyFloat(AI_CONFIG_APP_SCALE_KEY, 0.0f);
	if (!std::isfinite(metersPerSourceUnit) || metersPerSourceUnit <= 0.0f)
	{
		throw Diagnostics::Error("FBX source does not provide a valid linear-unit conversion to metres.");
	}

	return metersPerSourceUnit;
}

SourceImportOutput ImportFbxScene(const std::filesystem::path& filePath)
{
	SourceImportOutput output;

	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	const aiScene& scene = AssimpSceneReader::LoadScene(filePath, importer);
	const float sourceMetersPerUnit = ReadSourceMetersPerUnit(importer);
	output.provenance.sourceMetersPerUnit = sourceMetersPerUnit;
	output.scene.materials.reserve(scene.mNumMaterials);
	const std::size_t importedMeshInstanceCount = AssimpGeometryImporter::CountImportedMeshInstances(*scene.mRootNode);
	output.scene.meshPrimitives.reserve(scene.mNumMeshes);
	output.scene.meshInstances.reserve(importedMeshInstanceCount);

	const std::vector<std::filesystem::path> embeddedTexturePaths = FbxEmbeddedTextureImporter::ExtractTextures(scene);
	AssimpMaterialImporter::ImportMaterials(scene, filePath.parent_path(), embeddedTexturePaths, output.scene);

	AssimpGeometryImporter::ImportGeometry(scene, output.scene);
	FbxCameraImporter::ImportCameras(scene, sourceMetersPerUnit, output.scene);
	FbxLightImporter::ImportLights(scene, sourceMetersPerUnit, output.scene);
	FbxAnimationImporter::ImportAnimations(scene, output.scene);

	return output;
}
