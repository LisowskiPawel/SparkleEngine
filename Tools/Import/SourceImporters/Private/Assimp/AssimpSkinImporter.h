#pragma once

#include "Types/ImportedScene.h"

#include <assimp/mesh.h>
#include <assimp/scene.h>

#include <cstdint>

class AssimpSkinImporter final
{
public:
	static ImportedSkeletonIndex ImportSkeleton(const aiScene& scene, const aiNode& meshNode, const aiMesh& mesh, std::uint32_t sourceMeshIndex, ImportedScene& importedScene);
	static void ImportSkinInfluences(const aiMesh& mesh, const ImportedSkeleton& skeleton, ImportedMeshGeometry& geometry);

private:
	struct SkeletonBuildState;

	static void CollectSkeletonTopology(const aiScene& scene, const aiNode& meshNode, const aiMesh& mesh, SkeletonBuildState& state);
	static void InitializeSkeleton(const aiScene& scene, const aiMesh& mesh, std::uint32_t sourceMeshIndex, SkeletonBuildState& state);
	static void AppendSkeletonJoints(const aiScene& scene, const aiMesh& mesh, SkeletonBuildState& state);
};
