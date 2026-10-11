#pragma once

#include "Core/Public/Paths/ProductUserStatePaths.h"

namespace Filesystem::Private
{
	ProductUserStatePaths ResolveCurrentProductUserStatePaths();
	std::filesystem::path ResolveSparkleUserStateRoot();
}
