#include "PCH.h"

#include "Paths/FileSystemDiscovery.h"

#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Json/JsonReader.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Paths/UserStatePaths.h"

#include <system_error>

namespace Filesystem::Private
{
	static bool HasReparsePath(const std::filesystem::path& root, const std::filesystem::path& relative)
	{
		std::filesystem::path current = root;
		for (const auto& part : relative)
		{
			current /= part;
#if defined(_WIN32)
			const DWORD attributes = GetFileAttributesW(current.c_str());
			if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0u)
			{
				return true;
			}
#else
			std::error_code errorCode;
			if (std::filesystem::is_symlink(std::filesystem::symlink_status(current, errorCode)))
			{
				return true;
			}
#endif
		}
		return false;
	}

	static std::optional<std::filesystem::path> ReadPackageRelativePath(const std::filesystem::path& packageRoot, std::string_view property)
	{
		if (HasReparsePath(packageRoot, "manifests/sparkle-package-manifest.json"))
		{
			return std::nullopt;
		}
		const std::filesystem::path manifestPath = packageRoot / "manifests" / "sparkle-package-manifest.json";
		std::error_code errorCode;
		const auto size = std::filesystem::file_size(manifestPath, errorCode);
		if (errorCode || size > 16u * 1024u * 1024u)
		{
			return std::nullopt;
		}
		std::string document;
		std::string failure;
		std::string relative;
		if (!Files::TryReadAllText(manifestPath, document, failure) || !Json::TryReadStringProperty(document, property, relative) || relative.empty()
		    || relative.find_first_of("\\:") != std::string::npos || relative.front() == '/' || relative.back() == '/')
		{
			return std::nullopt;
		}
		const std::filesystem::path path = Strings::ToWide(relative);
		for (const auto& part : path)
		{
			const std::string name = Strings::ToNarrow(part.wstring());
			if (name.empty() || name == "." || name == ".." || name.find_first_of("<>\"|?*") != std::string::npos || name.back() == '.' || name.back() == ' ')
			{
				return std::nullopt;
			}
		}
		return path;
	}

	static std::optional<std::filesystem::path> ResolveContainedPackageDirectory(const std::filesystem::path& packageRoot, std::string_view property)
	{
		const auto relative = ReadPackageRelativePath(packageRoot, property);
		if (!relative || HasReparsePath(packageRoot, *relative))
		{
			return std::nullopt;
		}
		const std::filesystem::path result = Paths::Normalize(packageRoot / *relative);
		std::error_code errorCode;
		if (!Paths::IsUnderRoot(result, packageRoot) || !std::filesystem::is_directory(result, errorCode) || errorCode)
		{
			return std::nullopt;
		}
		return result;
	}

	std::optional<std::filesystem::path> DiscoverPackageProjectRoot(const std::filesystem::path& packageRoot)
	{
		return ResolveContainedPackageDirectory(packageRoot, "ProjectPath");
	}

	std::optional<std::filesystem::path> DiscoverPackageEngineRoot(const std::filesystem::path& packageRoot)
	{
		return ResolveContainedPackageDirectory(packageRoot, "EnginePath");
	}

	std::optional<std::filesystem::path> ResolvePackageContentRoot(const std::filesystem::path& packageRoot)
	{
		const auto relative = ReadPackageRelativePath(packageRoot, "ContentStore");
		if (!relative)
		{
			return std::nullopt;
		}
		const std::filesystem::path userStateRoot = ResolveSparkleUserStateRoot();
		if (HasReparsePath(userStateRoot, *relative))
		{
			return std::nullopt;
		}
		const std::filesystem::path result = Paths::Normalize(userStateRoot / *relative);
		return Paths::IsUnderRoot(result, userStateRoot) ? std::optional(result) : std::nullopt;
	}
}
