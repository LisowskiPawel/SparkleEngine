#include "PCH.h"

#include "Core/Public/Paths/PackagePaths.h"
#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Paths/WorkspaceOutputPaths.h"
#include "Core/Public/Hash/HashUtils.h"

#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Json/JsonReader.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Paths/UserStatePaths.h"

#include <system_error>

namespace Filesystem
{
	static bool HasReparsePath(const std::filesystem::path& root, const std::filesystem::path& relative)
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

	bool TryReadPackageManifest(const std::filesystem::path& packageRoot, std::string& outDocument)
	{
		if (HasReparsePath(packageRoot.parent_path(), packageRoot.filename() / "manifests/sparkle-package-manifest.json"))
		{
			return false;
		}
		std::error_code errorCode;
		const auto manifestPath = packageRoot / "manifests/sparkle-package-manifest.json";
		const auto size = std::filesystem::file_size(manifestPath, errorCode);
		std::string failure;
		return !errorCode && size <= 16u * 1024u * 1024u && Files::TryReadAllText(manifestPath, outDocument, failure);
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

	static std::optional<std::filesystem::path> ReadPackageRelativePath(const std::filesystem::path& packageRoot, std::string_view property)
	{
		std::string document;
		std::string relative;
		return TryReadPackageManifest(packageRoot, document) && Json::TryReadStringProperty(document, property, relative) ? ParsePackageRelativePath(relative) : std::nullopt;
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

		const std::filesystem::path userStateRoot = Private::ResolveSparkleUserStateRoot();
		if (HasReparsePath(userStateRoot, *relative))
		{
			return std::nullopt;
		}

		const std::filesystem::path result = Paths::Normalize(userStateRoot / *relative);
		return Paths::IsUnderRoot(result, userStateRoot) ? std::optional(result) : std::nullopt;
	}

	std::optional<std::filesystem::path> DiscoverPackageRoot(const std::filesystem::path& startPath)
	{
		const auto executableDirectory = Filesystem::GetExecutableDirectory();
		if (startPath.empty() || Paths::Normalize(startPath) == Paths::Normalize(executableDirectory))
		{
			if (const auto source = FindAncestorWithMarker(executableDirectory, kWorkspaceMarker))
			{
				const auto outputs = ResolveWorkspaceOutputPaths(*source);
				const auto relative = executableDirectory.lexically_relative(outputs.ArtifactRoot);
				auto component = relative.begin();
				if (component != relative.end() && *component != "..")
				{
					const bool canonical = *component == "dev";
					++component;
					if (canonical || (component != relative.end() && *component == "dev"))
					{
						return std::nullopt;
					}
				}
			}
			const auto profile = executableDirectory.filename();
			if (profile == "ShippingEditor" || profile == "ShippingGame")
			{
				const auto productDirectory = executableDirectory.parent_path();
				if (productDirectory.filename() == "Binaries"
				    && (productDirectory.parent_path().parent_path().filename() == "Projects" || productDirectory.parent_path().parent_path().filename() == "Tools"))
				{
					return Paths::Normalize(productDirectory.parent_path().parent_path().parent_path());
				}
			}
		}
		std::error_code ec;
		auto currentDir = std::filesystem::weakly_canonical((startPath.empty() ? Filesystem::GetExecutableDirectory() : startPath), ec);
		if (ec)
		{
			currentDir = (startPath.empty() ? Filesystem::GetExecutableDirectory() : startPath);
		}

		for (uint32_t depth = 0; depth < 8 && !currentDir.empty(); ++depth)
		{
			const bool hasPackageManifest = std::filesystem::exists(currentDir / "manifests" / "sparkle-package-manifest.json", ec);
			ec.clear();
			if (hasPackageManifest)
			{
				return Paths::Normalize(currentDir);
			}

			const auto parentDir = currentDir.parent_path();
			if (parentDir == currentDir)
			{
				break;
			}
			currentDir = parentDir;
		}

		return std::nullopt;
	}

	std::optional<std::filesystem::path> ResolvePackageFilePath(const std::filesystem::path& packageRoot, std::string_view relativePath, std::string_view manifest)
	{
		const auto relative = ParsePackageRelativePath(relativePath);
		if (!relative || HasReparsePath(packageRoot, *relative))
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
		std::uint64_t size = 0u;
		std::string expectedHash;
		if (entry.empty() || !Json::TryReadUInt64Property(entry, "Size", size) || !Json::TryReadStringProperty(entry, "Sha256", expectedHash))
		{
			return std::nullopt;
		}
		const auto path = Paths::Normalize(packageRoot / *relative);
		std::error_code errorCode;
		Hash::Sha256Digest hash;
		std::string failure;
		if (!Paths::IsUnderRoot(path, packageRoot) || std::filesystem::file_size(path, errorCode) != size || errorCode || !Hash::TrySha256File(path, hash, failure)
		    || Hash::Sha256ToHex(hash) != expectedHash)
		{
			return std::nullopt;
		}
		return path;
	}

	std::optional<std::filesystem::path> ResolvePackageSourceRoot(const std::filesystem::path& packageRoot)
	{
		const auto content = ResolvePackageContentRoot(packageRoot);
		const auto project = DiscoverPackageProjectRoot(packageRoot);
		if (!content || !project)
		{
			return std::nullopt;
		}
		const auto relative = std::filesystem::path("Source") / project->filename();
		return HasReparsePath(*content, relative) ? std::nullopt : std::optional(*content / relative);
	}

	std::optional<std::filesystem::path> ResolvePackageProductPath(const std::filesystem::path& packageRoot, std::string_view target, std::string_view configuration)
	{
		std::string document;
		std::vector<std::string_view> products;
		if (!TryReadPackageManifest(packageRoot, document) || !Json::TryReadArrayProperty(document, "Products", products))
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
		return ResolvePackageFilePath(packageRoot, selectedPath, document);
	}
}
