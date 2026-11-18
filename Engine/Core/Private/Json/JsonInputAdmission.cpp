#include "PCH.h"

#include "Core/Public/Json/JsonInputAdmission.h"

#include "Core/Public/Diagnostics/Error.h"

namespace Json
{
	InputAdmission::InputAdmission(std::string_view input, std::size_t maximumBytes)
	{
		if (input.size() > maximumBytes || input.find('\0') != std::string_view::npos)
		{
			throw Diagnostics::Error("JSON input exceeds its byte limit or contains a raw NUL.");
		}
	}

	void InputAdmission::CheckDepth(int depth) const
	{
		if (depth > 65)
		{
			throw Diagnostics::Error("JSON input exceeds its nesting limit.");
		}
	}

	void InputAdmission::BeginObject(int depth)
	{
		CheckDepth(depth);
		m_keys.resize(static_cast<std::size_t>(depth) + 1);
		m_keys.back().clear();
	}

	void InputAdmission::EndObject(int depth)
	{
		m_keys.resize(static_cast<std::size_t>(depth));
	}

	void InputAdmission::ReadKey(int depth, const std::string& key)
	{
		if (!TryReadKey(depth, key))
		{
			throw Diagnostics::Error("JSON input repeats a consumed member.");
		}
	}

	bool InputAdmission::TryReadKey(int depth, const std::string& key)
	{
		return m_keys.at(static_cast<std::size_t>(depth) - 1).insert(key).second;
	}

}
