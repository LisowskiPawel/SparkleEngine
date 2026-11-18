#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstddef>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

namespace Files::BinaryStreamWriter
{
	SPARKLE_CORE_API bool WriteBytes(std::ostream& output, const void* bytes, std::size_t byteCount, std::string& outErrorMessage);

	template <typename T> bool WriteValue(std::ostream& output, const T& value, std::string& outErrorMessage)
	{
		static_assert(std::is_trivially_copyable_v<T>, "BinaryStreamWriter requires trivially-copyable values");
		return WriteBytes(output, &value, sizeof(T), outErrorMessage);
	}

	template <typename T> bool WriteArray(std::ostream& output, const std::vector<T>& values, std::string& outErrorMessage)
	{
		static_assert(std::is_trivially_copyable_v<T>, "BinaryStreamWriter requires trivially-copyable values");
		return WriteBytes(output, values.data(), sizeof(T) * values.size(), outErrorMessage);
	}
}
