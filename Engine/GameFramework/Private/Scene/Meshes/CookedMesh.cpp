#include "PCH.h"
#include "Scene/Meshes/CookedMesh.h"
#include <utility>

CookedMesh::CookedMesh(MeshData&& meshData) noexcept :
    m_meshData(std::move(meshData))
{
}

CookedMesh::CookedMesh(MeshData&& meshData, Assets::CookedAssetId assetId) noexcept :
    m_meshData(std::move(meshData)),
    m_assetId(assetId)
{
}

CookedMesh::~CookedMesh() = default;

CookedMesh::CookedMesh(CookedMesh&&) noexcept = default;

CookedMesh& CookedMesh::operator=(CookedMesh&&) noexcept = default;
