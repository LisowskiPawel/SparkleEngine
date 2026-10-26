#include "PCH.h"

#include "PackageManifest.h"

#include "Core/Public/Paths/PackagePaths.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Json/JsonReader.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Strings/StringUtils.h"

#include <algorithm>
#include <cstdint>
#include <system_error>
#include <utility>
#include <vector>

#if defined(_WIN32)
  #include <Windows.h>
#endif

namespace Filesystem::Private
{
	bool HasReparsePath(const std::filesystem::path& root, const std::filesystem::path& relative)
	{
		const auto path = std::filesystem::absolute(root / relative).lexically_normal();
		std::filesystem::path current = path.root_path();
		for (const auto& part : path.relative_path())
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

	static std::optional<std::filesystem::path> ParsePackageRelativePath(std::string_view relative)
	{
		if (relative.empty() || relative.find_first_of("\\:") != std::string_view::npos || relative.find("//") != std::string_view::npos || relative.front() == '/' || relative.back() == '/'
		    || std::any_of(relative.begin(), relative.end(), [](unsigned char character) { return character < 0x20u; }))
		{
			return std::nullopt;
		}

		const std::filesystem::path path = Strings::ToWide(std::string_view(relative));
		for (const auto& part : path)
		{
			const std::string name = Strings::ToNarrow(part.wstring());
			const std::string device = Strings::ToLowerCopy(std::string_view(name).substr(0u, name.find('.')));
			if (name.empty() || name == "." || name == ".." || name.find_first_of("<>\"|?*") != std::string::npos || name.back() == '.' || name.back() == ' ')
			{
				return std::nullopt;
			}
			if (device == "con" || device == "prn" || device == "aux" || device == "nul"
			    || (device.size() == 4u && (device.starts_with("com") || device.starts_with("lpt")) && device.back() >= '1' && device.back() <= '9'))
			{
				return std::nullopt;
			}
		}
		return path;
	}

	std::optional<std::filesystem::path> ReadPackageRelativePath(const std::filesystem::path& packageRoot, std::string_view property)
	{
		std::string document;
		std::string relative;
		return TryReadPackageManifest(packageRoot, document) && Json::TryReadStringProperty(document, property, relative) ? ParsePackageRelativePath(relative) : std::nullopt;
	}

	std::optional<std::filesystem::path> ResolveContainedPackageDirectory(const std::filesystem::path& packageRoot, std::string_view property)
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

}

namespace Filesystem
{
	bool TryReadPackageManifest(const std::filesystem::path& packageRoot, std::string& outDocument)
	{
		if (Private::HasReparsePath(packageRoot.parent_path(), packageRoot.filename() / "manifests/sparkle-package-manifest.json"))
		{
			return false;
		}
		std::error_code errorCode;
		const auto manifestPath = packageRoot / "manifests/sparkle-package-manifest.json";
		const auto size = std::filesystem::file_size(manifestPath, errorCode);
		std::string failure;
		return !errorCode && size <= 16u * 1024u * 1024u && Files::TryReadAllText(manifestPath, outDocument, failure);
	}

	static bool ValidatePackageFileEntry(const std::filesystem::path& packageRoot, std::string_view file, std::string& outErrorMessage)
	{
		std::string name;
		if (!Json::TryReadStringProperty(file, "Path", name))
		{
			outErrorMessage = "Invalid package file entry.";
			return false;
		}
		const auto relative = Private::ParsePackageRelativePath(name);
		if (!relative || Private::HasReparsePath(packageRoot, *relative))
		{
			outErrorMessage = "Unsafe package file path.";
			return false;
		}

		std::error_code errorCode;
		if (!std::filesystem::is_regular_file(packageRoot / *relative, errorCode) || errorCode)
		{
			outErrorMessage = "Package file is missing or inaccessible: " + name;
			return false;
		}
		return true;
	}

	bool ValidatePackageFiles(const std::filesystem::path& packageRoot, std::string& outErrorMessage)
	{
		std::string document;
		std::vector<std::string_view> files;
		if (!TryReadPackageManifest(packageRoot, document) || !Json::TryReadArrayProperty(document, "Files", files) || files.empty())
		{
			outErrorMessage = "Invalid or missing package file list: manifests/sparkle-package-manifest.json";
			return false;
		}

		for (const auto file : files)
		{
			if (!ValidatePackageFileEntry(packageRoot, file, outErrorMessage))
			{
				return false;
			}
		}
		return true;
	}

	static std::optional<std::string_view> FindPackageFileEntry(std::string_view manifest, std::string_view relativePath)
	{
		std::vector<std::string_view> files;
		if (!Json::TryReadArrayProperty(manifest, "Files", files))
		{
			return std::nullopt;
		}
		std::string_view entry;
		for (const auto file : files)
		{
			std::string path;
			if (!Json::TryReadStringProperty(file, "Path", path))
			{
				return std::nullopt;
			}
			if (path == relativePath)
			{
				if (!entry.empty())
				{
					return std::nullopt;
				}
				entry = file;
			}
		}
		return entry.empty() ? std::nullopt : std::make_optional(entry);
	}

	static bool MatchesPackageFileIdentity(const std::filesystem::path& path, std::string_view entry)
	{
		std::uint64_t size = 0u;
		std::string expectedHash;
		if (!Json::TryReadUInt64Property(entry, "Size", size) || !Json::TryReadStringProperty(entry, "Sha256", expectedHash))
		{
			return false;
		}
		std::error_code errorCode;
		Hash::Sha256Digest hash;
		std::string failure;
		if (std::filesystem::file_size(path, errorCode) != size || errorCode || !Hash::TrySha256File(path, hash, failure) || Hash::Sha256ToHex(hash) != expectedHash)
		{
			return false;
		}
		return true;
	}

	std::optional<std::filesystem::path> ResolvePackageFilePath(const std::filesystem::path& packageRoot, std::string_view relativePath, std::string_view manifest)
	{
		const auto relative = Private::ParsePackageRelativePath(relativePath);
		if (!relative || Private::HasReparsePath(packageRoot, *relative))
		{
			return std::nullopt;
		}
		std::string document;
		if (manifest.empty())
		{
			if (!TryReadPackageManifest(packageRoot, document))
			{
				return std::nullopt;
			}
			manifest = document;
		}
		const auto entry = FindPackageFileEntry(manifest, relativePath);
		if (!entry)
		{
			return std::nullopt;
		}
		const auto path = Paths::Normalize(packageRoot / *relative);
		return Paths::IsUnderRoot(path, packageRoot) && MatchesPackageFileIdentity(path, *entry) ? std::make_optional(path) : std::nullopt;
	}

	static std::optional<std::string> FindPackageProductPath(std::string_view manifest, std::string_view target, std::string_view configuration)
	{
		std::vector<std::string_view> products;
		if (!Json::TryReadArrayProperty(manifest, "Products", products))
		{
			return std::nullopt;
		}
		std::string selectedPath;
		for (const auto product : products)
		{
			std::string productTarget;
			std::string profile;
			std::string path;
			if (!Json::TryReadStringProperty(product, "Target", productTarget) || !Json::TryReadStringProperty(product, "Configuration", profile)
			    || !Json::TryReadStringProperty(product, "Path", path))
			{
				return std::nullopt;
			}
			if (productTarget == target && profile == configuration)
			{
				if (!selectedPath.empty())
				{
					return std::nullopt;
				}
				selectedPath = std::move(path);
			}
		}
		return selectedPath;
	}

	std::optional<std::filesystem::path> ResolvePackageProductPath(const std::filesystem::path& packageRoot, std::string_view target, std::string_view configuration)
	{
		std::string document;
		if (!TryReadPackageManifest(packageRoot, document))
		{
			return std::nullopt;
		}
		const auto selectedPath = FindPackageProductPath(document, target, configuration);
		return selectedPath ? ResolvePackageFilePath(packageRoot, *selectedPath, document) : std::nullopt;
	}
}
