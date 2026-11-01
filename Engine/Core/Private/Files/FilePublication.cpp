#include "PCH.h"

#include "Core/Public/Files/FileUtils.h"

#include <Windows.h>

#include <format>
#include <fstream>

namespace Files
{
	std::filesystem::path BuildTemporaryPath(const std::filesystem::path& path, std::string_view suffix)
	{
		std::filesystem::path temporaryPath = path;
		temporaryPath += std::string(suffix.empty() ? std::string_view{".tmp"} : suffix);
		return temporaryPath;
	}

	bool TryFinalizeTemporaryFile(const std::filesystem::path& temporaryPath, const std::filesystem::path& finalPath, std::string& outErrorMessage)
	{
		if (MoveFileExW(temporaryPath.c_str(), finalPath.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
		{
			outErrorMessage.clear();
			return true;
		}
		outErrorMessage = std::format("Failed to move temporary file '{}' into place as '{}'", temporaryPath.string(), finalPath.string());
		return false;
	}

	bool TryFinalizeTemporaryFileIfMissing(const std::filesystem::path& temporaryPath, const std::filesystem::path& finalPath, std::string& outErrorMessage)
	{
		std::error_code errorCode;
		if (std::filesystem::exists(finalPath, errorCode) && !errorCode)
		{
			CleanupTemporaryFile(temporaryPath);
			outErrorMessage.clear();
			return true;
		}

		errorCode.clear();
		std::filesystem::rename(temporaryPath, finalPath, errorCode);
		if (!errorCode)
		{
			outErrorMessage.clear();
			return true;
		}

		std::error_code existsError;
		if (std::filesystem::exists(finalPath, existsError) && !existsError)
		{
			CleanupTemporaryFile(temporaryPath);
			outErrorMessage.clear();
			return true;
		}

		CleanupTemporaryFile(temporaryPath);
		outErrorMessage = std::format("Failed to move temporary file '{}' into place as '{}'", temporaryPath.string(), finalPath.string());
		return false;
	}

	void CleanupTemporaryFile(const std::filesystem::path& temporaryPath, std::ofstream* output) noexcept
	{
		if (output != nullptr && output->is_open())
		{
			output->close();
		}

		std::error_code errorCode;
		std::filesystem::remove(temporaryPath, errorCode);
	}
}
