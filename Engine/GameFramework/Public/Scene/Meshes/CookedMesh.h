#pragma once

#include "GameFramework/Public/Assets/Cooked/CookedAssetCommon.h"
#include "GameFramework/Public/GameFrameworkAPI.h"
#include "Mesh.h"

class SPARKLE_ENGINE_API CookedMesh final : public Mesh
{
public:
	explicit CookedMesh(MeshData&& meshData) noexcept;
	CookedMesh(MeshData&& meshData, Assets::CookedAssetId assetId) noexcept;

	~CookedMesh() override;

	CookedMesh(const CookedMesh&) = delete;
	CookedMesh& operator=(const CookedMesh&) = delete;
	CookedMesh(CookedMesh&&) noexcept;
	CookedMesh& operator=(CookedMesh&&) noexcept;

	Assets::CookedAssetId GetAssetId() const noexcept { return m_assetId; }

	const MeshData& GetMeshData() const noexcept override { return m_meshData; }

private:
	MeshData m_meshData;
	Assets::CookedAssetId m_assetId = Assets::InvalidCookedAssetId;
};
