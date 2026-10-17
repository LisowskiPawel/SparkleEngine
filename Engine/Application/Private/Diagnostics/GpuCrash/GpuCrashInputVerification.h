#pragma once

#include "RHI/Public/Core/RhiBackendSelection.h"

#include <filesystem>
#include <set>
#include <string>
#include <string_view>

namespace GpuCrashInputVerification
{
	void CheckLocalPath(const std::filesystem::path& path);
	std::string ReadReceipt(const std::filesystem::path& path);
	std::string ReadRequiredString(std::string_view json, std::string_view key);
	std::string HashFile(const std::filesystem::path& path);
	std::filesystem::path ResolveReceiptFile(const std::filesystem::path& root, std::string_view value);
	std::set<std::wstring> VerifyReceiptFiles(const std::filesystem::path& root, std::string_view receipt, std::string_view property);
	std::string VerifyShaderBundle(const std::filesystem::path& exportPath, std::string_view product, ERhiBackendApi api);
}
