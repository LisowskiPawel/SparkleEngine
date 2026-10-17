#pragma once

#include "Core/Public/CoreAPI.h"

#include <string>
#include <string_view>

namespace Identifiers
{
	SPARKLE_CORE_API std::string CreateUuidV4String();
	SPARKLE_CORE_API bool IsUuidV4String(std::string_view value) noexcept;
}
