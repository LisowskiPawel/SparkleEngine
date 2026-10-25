#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

namespace Files
{
	class BinarySpanReader final
	{
	public:
		explicit SPARKLE_CORE_API BinarySpanReader(std::span<const std::uint8_t> bytes) noexcept;

		template <typename T> bool ReadValue(T& outValue, std::string& outErrorMessage)
		{
			static_assert(std::is_trivially_copyable_v<T>, "BinarySpanReader requires trivially-copyable values");

			if (!CanRead(sizeof(T)))
			{
				outErrorMessage = "Unexpected end of binary data";
				return false;
			}

			std::memcpy(&outValue, m_bytes.data() + m_offset, sizeof(T));
			m_offset += sizeof(T);
			return true;
		}

		template <typename T> bool ReadArrayBytes(std::size_t elementCount, std::span<const std::uint8_t>& outBytes, std::string& outErrorMessage)
		{
			static_assert(std::is_trivially_copyable_v<T>, "BinarySpanReader requires trivially-copyable array elements");
			if (!TryGetArrayByteCount<T>(elementCount, outErrorMessage))
			{
				return false;
			}
			return ReadBytes(sizeof(T) * elementCount, outBytes, outErrorMessage);
		}

		template <typename T> bool ReadArray(std::size_t elementCount, std::vector<T>& outValues, std::string& outErrorMessage)
		{
			static_assert(std::is_trivially_copyable_v<T>, "BinarySpanReader requires trivially-copyable array elements");
			if (!TryGetArrayByteCount<T>(elementCount, outErrorMessage))
			{
				return false;
			}

			const std::size_t arrayByteCount = sizeof(T) * elementCount;
			if (!CanRead(arrayByteCount))
			{
				outErrorMessage = "Unexpected end of binary array data";
				return false;
			}

			outValues.resize(elementCount);
			if (arrayByteCount > 0)
			{
				std::memcpy(outValues.data(), m_bytes.data() + m_offset, arrayByteCount);
			}
			m_offset += arrayByteCount;
			return true;
		}

		SPARKLE_CORE_API bool ReadBytes(std::size_t byteCount, std::span<const std::uint8_t>& outBytes, std::string& outErrorMessage);
		SPARKLE_CORE_API bool ReadStringWithUInt32Length(std::string& outValue, std::string& outErrorMessage);
		SPARKLE_CORE_API bool ReadString(std::size_t byteCount, std::string& outValue, std::string& outErrorMessage);

		std::size_t GetOffset() const noexcept { return m_offset; }

		std::size_t GetRemainingByteCount() const noexcept { return m_bytes.size() - m_offset; }

	private:
		template <typename T> static bool TryGetArrayByteCount(std::size_t elementCount, std::string& outErrorMessage)
		{
			if (elementCount > (std::numeric_limits<std::size_t>::max)() / sizeof(T))
			{
				outErrorMessage = "Binary array size overflow";
				return false;
			}
			return true;
		}

		SPARKLE_CORE_API bool CanRead(std::size_t byteCount) const noexcept;

		std::span<const std::uint8_t> m_bytes;
		std::size_t m_offset = 0;
	};
}
