#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace Json
{
	// Admission state only: the upstream parser owns syntax and values.
	// Owners call ReadKey only for consumed members; unknown duplicates remain allowed.
	class InputAdmission final
	{
	public:
		SPARKLE_CORE_API explicit InputAdmission(std::string_view input, std::size_t maximumBytes);

		template <typename TEvent> void ReadEvent(int depth, TEvent event)
		{
			CheckDepth(depth);
			if (event == TEvent::object_start)
			{
				m_keys.resize(static_cast<std::size_t>(depth) + 1);
				m_keys.back().clear();
			}
			else if (event == TEvent::object_end)
			{
				m_keys.resize(static_cast<std::size_t>(depth));
			}
		}

		SPARKLE_CORE_API bool ReadKey(int depth, const std::string& key);
		SPARKLE_CORE_API bool TryReadKey(int depth, const std::string& key);

	private:
		SPARKLE_CORE_API void CheckDepth(int depth) const;

		std::vector<std::unordered_set<std::string>> m_keys;
	};
}
