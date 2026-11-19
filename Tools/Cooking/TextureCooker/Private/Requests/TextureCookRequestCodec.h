#pragma once

#include "TextureCookRequestList.h"

#include <string>
#include <string_view>

namespace TextureCookRequestCodec
{
	inline constexpr std::string_view Header = "TextureCookRequests|1";
	TextureCookRequest ParseLine(std::string_view line);
	std::string FormatLine(const TextureCookRequest& request);
}
