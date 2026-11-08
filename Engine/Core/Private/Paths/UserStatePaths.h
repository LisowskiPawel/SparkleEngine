#pragma once

#include "Core/Public/Paths/ProductUserStatePaths.h"

namespace Filesystem
{
	ProductUserStatePaths ResolveCurrentProductUserStatePaths();
	std::filesystem::path ResolveSparkleUserStateRoot();
}
