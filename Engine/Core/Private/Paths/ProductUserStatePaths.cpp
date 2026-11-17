#include "PCH.h"

#include "Core/Public/Paths/ProductUserStatePaths.h"

#include "UserStatePaths.h"

namespace Filesystem
{
	const ProductUserStatePaths& GetProductUserStatePaths()
	{
		static const ProductUserStatePaths paths = ResolveCurrentProductUserStatePaths();
		return paths;
	}
}
