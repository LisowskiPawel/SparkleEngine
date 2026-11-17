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
	class SPARKLE_CORE_API InputAdmission final
	{
	public:
		explicit InputAdmission(std::string_view input, std::size_t maximumBytes);
		void CheckDepth(int depth) const;
		void BeginObject(int depth);
		void EndObject(int depth);
		void ReadKey(int depth, const std::string& key);

	private:
		std::vector<std::unordered_set<std::string>> m_keys;
	};
}
