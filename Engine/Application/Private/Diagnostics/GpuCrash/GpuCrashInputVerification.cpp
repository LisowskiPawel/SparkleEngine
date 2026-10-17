#include "PCH.h"

#include "Diagnostics/GpuCrash/GpuCrashInputVerification.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Json/JsonReader.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Strings/StringUtils.h"

#include <Windows.h>
#include <vector>

namespace GpuCrashInputVerification
{
	void CheckLocalPath(const std::filesystem::path& path)
	{
		if (!path.is_absolute() || path.native().starts_with(L"\\\\"))
		{
			throw Diagnostics::Error("GPU crash receipts require local absolute paths.");
		}
		for (auto current = path; !current.empty(); current = current.parent_path())
		{
			const auto attributes = GetFileAttributesW(current.c_str());
			if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
			{
				throw Diagnostics::Error("GPU crash receipt path is missing or traverses a reparse point.");
			}
			if (current == current.root_path())
			{
				break;
			}
		}
	}

	std::string ReadReceipt(const std::filesystem::path& path)
	{
		CheckLocalPath(path);
		std::string text;
		std::string error;
		if (!Files::TryReadAllText(path, text, error, 32u * 1024u * 1024u))
		{
			throw Diagnostics::Error("GPU crash receipt is unreadable or exceeds its size limit.");
		}
		return text;
	}

	std::string ReadRequiredString(std::string_view json, std::string_view key)
	{
		std::string value;
		if (!Json::TryReadStringProperty(json, key, value) || value.empty())
		{
			throw Diagnostics::Error("GPU crash receipt has a missing or ambiguous identity field.");
		}
		return value;
	}

	std::string HashFile(const std::filesystem::path& path)
	{
		CheckLocalPath(path);
		Hash::Sha256Digest digest;
		std::string error;
		if (!Hash::TrySha256File(path, digest, error))
		{
			throw Diagnostics::Error("Cannot hash a GPU crash input.");
		}
		return Hash::Sha256ToHex(digest);
	}

	std::filesystem::path ResolveReceiptFile(const std::filesystem::path& root, std::string_view value)
	{
		const std::filesystem::path relative = Strings::ToWide(value);
		if (relative.empty() || relative.is_absolute() || relative.has_root_path() || relative.generic_string().find(':') != std::string::npos)
		{
			throw Diagnostics::Error("GPU crash receipt requires relative file membership.");
		}
		for (const auto& part : relative)
		{
			if (part == ".." || part == ".")
			{
				throw Diagnostics::Error("GPU crash receipt file escapes its owner.");
			}
		}
		return root / relative;
	}

	std::set<std::wstring> VerifyReceiptFiles(const std::filesystem::path& root, std::string_view receipt, std::string_view property)
	{
		std::vector<std::string_view> files;
		if (!Json::TryReadArrayProperty(receipt, property, files) || (property == "files" && files.empty()) || files.size() > 65'536)
		{
			throw Diagnostics::Error("GPU crash file membership is missing or exceeds its bound.");
		}
		std::set<std::wstring> verified;
		for (const auto file : files)
		{
			const auto path = ResolveReceiptFile(root, ReadRequiredString(file, "path"));
			std::uint64_t bytes = 0;
			if (!Json::TryReadUInt64Property(file, "bytes", bytes) || !verified.insert(Paths::MakePathKey(path)).second || HashFile(path) != ReadRequiredString(file, "sha256")
			    || std::filesystem::file_size(path) != bytes)
			{
				throw Diagnostics::Error("GPU crash input is stale, duplicated or incomplete.");
			}
		}
		return verified;
	}
}
