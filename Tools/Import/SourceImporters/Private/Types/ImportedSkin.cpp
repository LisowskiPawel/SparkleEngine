#include "PCH.h"

#include "Types/ImportedSkin.h"

bool ImportedSkeleton::IsValid() const noexcept
{
	return !joints.empty();
}
