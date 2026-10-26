#pragma once

#include <string>

namespace Filesystem::Private
{
	std::string GetExecutableStem();
	std::string InferProjectNameFromExecutableStem(std::string executableStem);
}
