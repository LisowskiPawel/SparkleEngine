#include "PCH.h"

#include "Core/Public/Files/FileUtils.h"

#include <Windows.h>

#include <algorithm>
#include <format>
#include <fstream>

namespace Files
{
	bool TryWriteNewFileDurable(const std::filesystem::path& path, std::span<const std::byte> bytes, std::string& outErrorMessage)
	{
		const HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, nullptr);
		if (file == INVALID_HANDLE_VALUE)
		{
			outErrorMessage = "Cannot create durable output file.";
			return false;
		}
		std::size_t offset = 0;
		bool success = true;
		while (offset < bytes.size())
		{
			const DWORD count = static_cast<DWORD>(std::min<std::size_t>(bytes.size() - offset, 16u * 1024u * 1024u));
			DWORD written = 0;
			if (!WriteFile(file, bytes.data() + offset, count, &written, nullptr) || written != count)
			{
				success = false;
				break;
			}
			offset += written;
		}
		if (success)
		{
			success = FlushFileBuffers(file) != FALSE;
		}
		const bool closed = CloseHandle(file) != FALSE;
		if (!success || !closed)
		{
			outErrorMessage = "Durable output did not finish writing, flushing and closing.";
			return false;
		}
		outErrorMessage.clear();
		return true;
	}

	static bool TryOpenOutput(const std::filesystem::path& path, std::ios::openmode mode, std::ofstream& output, std::string& outErrorMessage)
	{
		std::error_code errorCode;
		if (path.has_parent_path())
		{
			std::filesystem::create_directories(path.parent_path(), errorCode);
			if (errorCode)
			{
				outErrorMessage = std::format("Failed to create directories for '{}': {}", path.string(), errorCode.message());
				return false;
			}
		}

		output.open(path, mode | std::ios::trunc);
		if (!output)
		{
			outErrorMessage = std::format("Failed to open '{}' for writing", path.string());
			return false;
		}

		outErrorMessage.clear();
		return true;
	}

	bool TryWriteAllBytes(const std::filesystem::path& path, std::span<const std::byte> bytes, std::string& outErrorMessage)
	{
		std::ofstream output;
		if (!TryOpenBinaryOutput(path, output, outErrorMessage))
		{
			return false;
		}
		if (!bytes.empty() && !output.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size())))
		{
			outErrorMessage = std::format("Failed to write '{}'", path.string());
			return false;
		}

		output.flush();
		if (!output)
		{
			outErrorMessage = std::format("Failed to flush '{}'", path.string());
			return false;
		}
		outErrorMessage.clear();
		return true;
	}

	bool TryWriteAllBytes(const std::filesystem::path& path, const std::vector<std::uint8_t>& bytes, std::string& outErrorMessage)
	{
		return TryWriteAllBytes(path, std::as_bytes(std::span(bytes)), outErrorMessage);
	}

	bool TryWriteAllBytesAtomic(const std::filesystem::path& path, std::span<const std::byte> bytes, std::string& outErrorMessage)
	{
		const std::filesystem::path temporaryPath = BuildTemporaryPath(path);
		CleanupTemporaryFile(temporaryPath);
		if (!TryWriteAllBytes(temporaryPath, bytes, outErrorMessage) || !TryFinalizeTemporaryFile(temporaryPath, path, outErrorMessage))
		{
			CleanupTemporaryFile(temporaryPath);
			return false;
		}
		return true;
	}

	bool TryWriteAllText(const std::filesystem::path& path, std::string_view text, std::string& outErrorMessage)
	{
		std::ofstream output;
		if (!TryOpenBinaryOutput(path, output, outErrorMessage))
		{
			return false;
		}
		if (!text.empty() && !output.write(text.data(), static_cast<std::streamsize>(text.size())))
		{
			outErrorMessage = std::format("Failed to write '{}'", path.string());
			return false;
		}

		output.flush();
		if (!output)
		{
			outErrorMessage = std::format("Failed to flush '{}'", path.string());
			return false;
		}
		outErrorMessage.clear();
		return true;
	}

	bool TryWriteAllTextAtomic(const std::filesystem::path& path, std::string_view text, std::string& outErrorMessage)
	{
		const std::filesystem::path temporaryPath = BuildTemporaryPath(path);
		CleanupTemporaryFile(temporaryPath);

		if (!TryWriteAllText(temporaryPath, text, outErrorMessage))
		{
			return false;
		}
		if (!TryFinalizeTemporaryFile(temporaryPath, path, outErrorMessage))
		{
			CleanupTemporaryFile(temporaryPath);
			return false;
		}

		outErrorMessage.clear();
		return true;
	}

	bool TryOpenBinaryOutput(const std::filesystem::path& path, std::ofstream& output, std::string& outErrorMessage)
	{
		return TryOpenOutput(path, std::ios::binary, output, outErrorMessage);
	}

	bool TryOpenTextOutput(const std::filesystem::path& path, std::ofstream& output, std::string& outErrorMessage)
	{
		return TryOpenOutput(path, static_cast<std::ios::openmode>(0), output, outErrorMessage);
	}

	bool TryCloseOutput(std::ofstream& output, const std::filesystem::path& path, std::string& outErrorMessage)
	{
		output.close();
		if (!output.fail())
		{
			outErrorMessage.clear();
			return true;
		}

		outErrorMessage = std::format("Failed to finalize output '{}'", path.string());
		return false;
	}
}
