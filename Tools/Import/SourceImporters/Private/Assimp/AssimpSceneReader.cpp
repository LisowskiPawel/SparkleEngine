#include "PCH.h"

#include "Assimp/AssimpSceneReader.h"

#include "Core/Public/Diagnostics/Error.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <format>

constexpr unsigned int AssimpSceneReader::GetPostProcessFlags() noexcept
{
	return aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_SortByPType | aiProcess_ValidateDataStructure | aiProcess_ImproveCacheLocality
	    | aiProcess_GlobalScale | aiProcess_ConvertToLeftHanded;
}

void AssimpSceneReader::ValidateInputPath(const std::filesystem::path& filePath)
{
	if (!std::filesystem::exists(filePath))
	{
		throw Diagnostics::Error(std::format("Assimp source file does not exist: '{}'.", filePath.string()));
	}
}

const aiScene& AssimpSceneReader::LoadScene(const std::filesystem::path& filePath, Assimp::Importer& importer)
{
	ValidateInputPath(filePath);

	const aiScene* scene = importer.ReadFile(filePath.string(), GetPostProcessFlags());
	if (scene == nullptr || scene->mRootNode == nullptr)
	{
		throw Diagnostics::Error(std::format("Cannot parse Assimp source '{}' ({}).", filePath.string(), importer.GetErrorString()));
	}

	return *scene;
}
