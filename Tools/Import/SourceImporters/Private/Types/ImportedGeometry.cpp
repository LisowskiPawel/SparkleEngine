#include "PCH.h"

#include "Types/ImportedGeometry.h"

bool ImportedMeshGeometry::IsValid() const noexcept
{
	return !vertices.empty() && !indices.empty();
}

bool ImportedMeshInstanceGroup::HasInstanceRange() const noexcept
{
	return firstInstanceIndex != kInvalidImportedMeshInstanceIndex && instanceCount > 0;
}
