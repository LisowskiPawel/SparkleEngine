#include "PackageContentStore.h"

#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Json/JsonReader.h"
#include "Core/Public/Paths/PackagePaths.h"
#include "Core/Public/Strings/StringUtils.h"

#include <algorithm>
#include <chrono>
#include <array>
#include <system_error>
#include <vector>

namespace SparkleLauncher
{
	struct SeedFile final
	{
		std::filesystem::path Source;
		std::filesystem::path Relative;
		std::string_view Entry;
	};

	static std::error_code CopyContentStoreSeed(const SeedFile& seed, const std::filesystem::path& transaction)
	{
		std::error_code errorCode;
		const auto destination = transaction / seed.Relative;
		std::filesystem::create_directories(destination.parent_path(), errorCode);
		if (errorCode)
		{
			return errorCode;
		}

		std::filesystem::copy_file(seed.Source, destination, std::filesystem::copy_options::none, errorCode);
		if (errorCode)
		{
			return errorCode;
		}

		std::filesystem::permissions(destination, std::filesystem::perms::owner_write, std::filesystem::perm_options::add, errorCode);
		if (errorCode)
		{
			return errorCode;
		}

		Hash::Sha256Digest hash;
		std::string expectedHash;
		std::string error;
		if (!Json::TryReadStringProperty(seed.Entry, "Sha256", expectedHash) || !Hash::TrySha256File(destination, hash, error) || Hash::Sha256ToHex(hash) != expectedHash)
		{
			return std::make_error_code(std::errc::io_error);
		}
		return {};
	}

	static bool PublishContentStore(const std::filesystem::path& contentRoot, const std::string& identity, const std::vector<SeedFile>& seeds, std::string& outErrorMessage)
	{
		std::error_code errorCode;
		const auto transaction = std::filesystem::path(contentRoot.wstring() + L".partial-" + std::to_wstring(std::chrono::steady_clock::now().time_since_epoch().count()));
		if (!std::filesystem::create_directories(transaction, errorCode) || errorCode)
		{
			outErrorMessage = "Cannot create content-store transaction: " + errorCode.message();
			return false;
		}

		for (const auto& directory : {"Meshes", "Textures", "Materials", "Skeletons", "Animations", "SceneManifests"})
		{
			std::filesystem::create_directories(transaction / "Cooked" / directory, errorCode);
			if (errorCode)
			{
				break;
			}
		}

		for (const auto& seed : seeds)
		{
			if (errorCode)
			{
				break;
			}
			errorCode = CopyContentStoreSeed(seed, transaction);
		}

		if (!errorCode && Files::TryWriteAllText(transaction / ".sparkle-package", identity, outErrorMessage))
		{
			std::filesystem::rename(transaction, contentRoot, errorCode);
			if (!errorCode)
			{
				return true;
			}
		}
		if (errorCode)
		{
			outErrorMessage = "Content-store initialization failed: " + errorCode.message();
		}

		std::error_code cleanupError;
		std::filesystem::remove_all(transaction, cleanupError);
		return false;
	}

	bool InitializePackageContentStore(const std::filesystem::path& packageRoot, std::string& outErrorMessage)
	{
		const auto content = Filesystem::ResolvePackageContentRoot(packageRoot);
		const auto source = Filesystem::ResolvePackageSourceRoot(packageRoot);
		const auto project = Filesystem::DiscoverPackageProjectRoot(packageRoot);
		const auto engine = Filesystem::DiscoverPackageEngineRoot(packageRoot);
		std::string document;
		std::string identity;
		std::vector<std::string_view> files;
		if (!content || !source || !project || !engine || !Filesystem::TryReadPackageManifest(packageRoot, document) || !Hash::TrySha256Hex(document, identity, outErrorMessage)
		    || !Json::TryReadArrayProperty(document, "Files", files))
		{
			outErrorMessage = "Invalid package manifest or content-store path.";
			return false;
		}

		const auto marker = *content / ".sparkle-package";
		std::error_code errorCode;
		if (std::filesystem::exists(*content, errorCode))
		{
			std::string retainedIdentity;
			if (Files::TryReadAllText(marker, retainedIdentity, outErrorMessage) && retainedIdentity == identity && std::filesystem::is_regular_file(*source / ".sparkle-project", errorCode)
			    && !errorCode && std::filesystem::is_regular_file(*source / "Levels.catalog", errorCode) && !errorCode)
			{
				return true;
			}
			outErrorMessage = "Content store belongs to a different or incomplete package. Remove it before initializing this candidate: " + Strings::ToNarrow(content->wstring());
			return false;
		}
		if (errorCode)
		{
			outErrorMessage = "Cannot inspect content store: " + errorCode.message();
			return false;
		}

		std::vector<SeedFile> seeds;
		const auto sourceRelative = source->lexically_relative(*content);
		const std::string projectPrefix = project->lexically_relative(packageRoot).generic_string() + "/";
		const std::string cookedPrefix = engine->lexically_relative(packageRoot).generic_string() + "/Cooked/";
		for (const auto file : files)
		{
			std::string path;
			if (!Json::TryReadStringProperty(file, "Path", path))
			{
				outErrorMessage = "Malformed package file entry.";
				return false;
			}
			std::filesystem::path destination;
			if (path.starts_with(projectPrefix))
			{
				const auto relative = std::string_view(path).substr(projectPrefix.size());
				if (relative.starts_with("Binaries/"))
				{
					continue;
				}
				destination = sourceRelative / Strings::ToWide(relative);
			}
			else if (path.starts_with(cookedPrefix))
			{
				destination = std::filesystem::path("Cooked") / Strings::ToWide(std::string_view(path).substr(cookedPrefix.size()));
			}
			else
			{
				continue;
			}
			const auto verified = Filesystem::ResolvePackageFilePath(packageRoot, path, document);
			if (!verified)
			{
				outErrorMessage = "Package content size/hash mismatch or unsafe path: " + path;
				return false;
			}
			seeds.push_back({*verified, std::move(destination), file});
		}

		const std::array required = {
		    sourceRelative / ".sparkle-project",
		    sourceRelative / "Levels.catalog",
		    std::filesystem::path("Cooked/Shaders/GlobalShaderMap.smap"),
		    std::filesystem::path("Cooked/Shaders/CookedShaderLibrary.slib"),
		    std::filesystem::path("Cooked/Shaders/ShaderDependencies.sdep")};

		for (const auto& path : required)
		{
			if (std::count_if(seeds.begin(), seeds.end(), [&path](const SeedFile& seed) { return seed.Relative == path; }) != 1)
			{
				outErrorMessage = "Missing or duplicate content-store seed: " + path.generic_string();
				return false;
			}
		}

		return PublishContentStore(*content, identity, seeds, outErrorMessage);
	}
}
