#include "PCH.h"
#include "Scene/Meshes/Mesh.h"

Mesh::~Mesh() = default;

Mesh::Mesh(Mesh&&) noexcept = default;

Mesh& Mesh::operator=(Mesh&&) noexcept = default;
