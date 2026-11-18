#pragma once

#include "RHI/Public/Core/RhiBackendSelection.h"

#include <nlohmann/json_fwd.hpp>

#include <filesystem>
#include <initializer_list>
#include <set>
#include <string>
#include <string_view>

namespace GpuCrashInputVerification
{
	void CheckLocalPath(const std::filesystem::path& path);
	nlohmann::json ReadReceipt(const std::filesystem::path& path, std::initializer_list<std::string_view> fields, std::string* rawBytes = nullptr);
	std::string HashFile(const std::filesystem::path& path);
	std::filesystem::path ResolveReceiptFile(const std::filesystem::path& root, std::string_view value);
	std::set<std::wstring> VerifyReceiptFiles(const std::filesystem::path& root, const nlohmann::json& files, bool bRequired);
	nlohmann::json VerifyShaderBundle(const std::filesystem::path& exportPath, const nlohmann::json& product, ERhiBackendApi api);
}
