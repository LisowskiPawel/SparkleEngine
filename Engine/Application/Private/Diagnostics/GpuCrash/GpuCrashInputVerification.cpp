#include "PCH.h"
#include "Core/Public/Hash/FileHash.h"

#include "Diagnostics/GpuCrash/GpuCrashInputVerification.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Json/JsonInputAdmission.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Strings/StringUtils.h"

#include <nlohmann/json.hpp>

#include <Windows.h>
#include <algorithm>

namespace GpuCrashInputVerification
{
	void CheckLocalPath(const std::filesystem::path& path)
	{
		const bool bInvalidLocation = !path.is_absolute() || path.native().starts_with(L"\\\\");
		const bool bEmbeddedNul = path.native().find(L'\0') != std::wstring::npos;
		if (bInvalidLocation || bEmbeddedNul)
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

	static nlohmann::json ParseReceipt(std::string_view text, std::initializer_list<std::string_view> fields)
	{
		Json::InputAdmission admission(text, 32u * 1024u * 1024u);
		bool bMembers = false;
		std::size_t memberCount = 0;

		auto document = nlohmann::json::parse(
		    text.begin(),
		    text.end(),
		    [&](int depth, nlohmann::json::parse_event_t event, nlohmann::json& value)
		    {
			    using Event = nlohmann::json::parse_event_t;
			    const bool bContainerStart = event == Event::object_start || event == Event::array_start;
			    const bool bEntry = bContainerStart || event == Event::value;
			    const bool bMemberEntry = bMembers && depth == 2;
			    const bool bCountMember = bMemberEntry && bEntry;
			    if (bCountMember && ++memberCount > 65'536)
			    {
				    throw Diagnostics::Error("GPU crash receipt exceeds its membership limit.");
			    }
			    if (event == Event::object_start)
			    {
				    admission.BeginObject(depth);
			    }
			    if (event == Event::array_start || event == Event::value)
			    {
				    admission.CheckDepth(depth);
			    }
			    if (event == Event::key)
			    {
				    const auto& key = value.get_ref<const std::string&>();
				    const bool bRootField = depth == 1 && std::find(fields.begin(), fields.end(), key) != fields.end();
				    const bool bFileIdentity = key == "path" || key == "sha256";
				    const bool bFileField = bFileIdentity || key == "bytes";
				    const bool bMemberDepth = bMembers && depth == 3;
				    const bool bMemberField = bMemberDepth && bFileField;
				    if (depth == 1)
				    {
					    const bool bFileArray = key == "files" || key == "runtimeFiles";
					    const bool bPathArray = key == "jobs" || key == "sources";
					    const bool bArray = bFileArray || bPathArray;
					    bMembers = bRootField && bArray;
					    memberCount = 0;
				    }
				    if (!bRootField && !bMemberField)
				    {
					    return false;
				    }
				    admission.ReadKey(depth, key);
			    }
			    if (event == Event::object_end)
			    {
				    admission.EndObject(depth);
			    }
			    return true;
		    });

		if (!document.is_object())
		{
			throw Diagnostics::Error("GPU crash receipt requires an object root.");
		}
		return document;
	}

	nlohmann::json ReadReceipt(const std::filesystem::path& path, std::initializer_list<std::string_view> fields, std::string* rawBytes)
	{
		CheckLocalPath(path);
		std::string text;
		std::string error;
		if (!Files::TryReadAllText(path, text, error, 32u * 1024u * 1024u))
		{
			throw Diagnostics::Error("GPU crash receipt is unreadable or exceeds its size limit.");
		}

		auto document = ParseReceipt(text, fields);
		if (rawBytes != nullptr)
		{
			*rawBytes = std::move(text);
		}
		return document;
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
		const bool bInvalidRelativePathIsabsolute = relative.empty() || relative.is_absolute();
		const bool bInvalidRelativePathRootpath = bInvalidRelativePathIsabsolute || (relative.has_root_path());
		const bool bInvalidCharacters = relative.generic_string().find(':') != std::string::npos || value.find('\0') != std::string_view::npos;
		const bool bInvalidReceiptRelativePath = bInvalidRelativePathRootpath || bInvalidCharacters;

		if (bInvalidReceiptRelativePath)
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

	std::set<std::wstring> VerifyReceiptFiles(const std::filesystem::path& root, const nlohmann::json& files, bool bRequired)
	{
		const bool bInvalidMembership = !files.is_array() || files.size() > 65'536;
		if (bInvalidMembership || (bRequired && files.empty()))
		{
			throw Diagnostics::Error("GPU crash file membership is missing or exceeds its bound.");
		}

		std::set<std::wstring> verified;
		for (const auto& file : files)
		{
			const auto path = ResolveReceiptFile(root, file.at("path").get_ref<const std::string&>());
			const auto& size = file.at("bytes");
			if (!size.is_number_unsigned())
			{
				throw Diagnostics::Error("GPU crash file size requires an unsigned integer.");
			}
			const auto bytes = size.get<std::uint64_t>();
			const bool bPathUnique = verified.insert(Paths::MakePathKey(path)).second;
			const bool bFileHashMatches = bPathUnique && HashFile(path) == file.at("sha256").get_ref<const std::string&>();
			const bool bFileSizeMatches = bFileHashMatches && std::filesystem::file_size(path) == bytes;
			if (!bFileSizeMatches)
			{
				throw Diagnostics::Error("GPU crash input is stale, duplicated or incomplete.");
			}
		}
		return verified;
	}
}
