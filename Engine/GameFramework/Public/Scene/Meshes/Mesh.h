#pragma once

#include "GameFramework/Public/GameFrameworkAPI.h"
#include "MeshData.h"

class SPARKLE_ENGINE_API Mesh
{
public:
	virtual ~Mesh();
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept;
	Mesh& operator=(Mesh&&) noexcept;

	virtual const MeshData& GetMeshData() const noexcept = 0;

protected:
	Mesh() = default;
};
