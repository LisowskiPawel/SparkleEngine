#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <type_traits>
#include <vector>

namespace Files
{
	class SPARKLE_CORE_API BinaryFileWriter final
	{
	public:
		explicit BinaryFileWriter(const std::filesystem::path& path);
		~BinaryFileWriter();
		BinaryFileWriter(const BinaryFileWriter&) = delete;
		BinaryFileWriter& operator=(const BinaryFileWriter&) = delete;

		template <typename T> void WriteValue(const T& value)
		{
			static_assert(std::is_trivially_copyable_v<T>, "BinaryFileWriter requires trivially-copyable values");
			WriteBytes(&value, sizeof(T));
		}

		template <typename T> void WriteArray(const std::vector<T>& values)
		{
			static_assert(std::is_trivially_copyable_v<T>, "BinaryFileWriter requires trivially-copyable values");
			WriteBytes(values.data(), sizeof(T) * values.size());
		}

		void WriteBytes(const void* bytes, std::size_t byteCount, std::string_view failureMessage = {});
		void Close();

	private:
		std::filesystem::path m_path;
		std::ofstream m_output;
	};

	template <typename THeader, typename... TElement> void WriteBinaryFile(const std::filesystem::path& path, const THeader& header, const std::vector<TElement>&... arrays)
	{
		BinaryFileWriter output(path);
		output.WriteValue(header);
		(output.WriteArray(arrays), ...);
		output.Close();
	}
}
