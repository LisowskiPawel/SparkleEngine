#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstddef>
#include <algorithm>
#include <initializer_list>
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

	// Optional metadata may keep unambiguous fields when another consumed field repeats.
	template <typename TDocument> TDocument ParseObjectFields(std::string_view input, std::size_t maximumBytes, std::initializer_list<std::string_view> fields)
	{
		InputAdmission admission(input, maximumBytes);
		std::unordered_set<std::string> repeated;
		const auto admitValue = [&](int depth, typename TDocument::parse_event_t event, TDocument& value)
		{
			admission.ReadEvent(depth, event);
			if (event != TDocument::parse_event_t::key)
			{
				return true;
			}

			const auto& key = value.template get_ref<const std::string&>();
			const bool bConsumed = depth == 1 && std::ranges::find(fields, key) != fields.end();
			if (bConsumed && !admission.TryReadKey(depth, key))
			{
				repeated.insert(key);
			}
			return bConsumed;
		};

		auto document = TDocument::parse(input.begin(), input.end(), admitValue);
		for (const auto& key : repeated)
		{
			document.erase(key);
		}
		return document;
	}
}
