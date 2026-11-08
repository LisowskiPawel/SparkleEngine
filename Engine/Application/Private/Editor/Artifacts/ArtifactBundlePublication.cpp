#include "PCH.h"
#include "Editor/Artifacts/ArtifactBundlePublication.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Paths/PathUtils.h"

#include <exception>
#include <limits>
#include <unordered_set>
#include <utility>

namespace ArtifactBundlePublication
{
	struct BundleContents final
	{
		const ArtifactBundleFile* Completion = nullptr;
		std::uintmax_t Bytes = 0;
	};

	static void CheckCancellation(std::stop_token token)
	{
		if (token.stop_requested())
		{
			throw Diagnostics::Error("Artifact publication was cancelled.");
		}
	}

	static bool IsRelativeFilePath(const std::filesystem::path& path)
	{
		const bool bRooted = path.empty() || path.has_root_path();
		if (bRooted || path.generic_string().find(':') != std::string::npos)
		{
			return false;
		}
		for (const auto& component : path)
		{
			if (component == "..")
			{
				return false;
			}
		}
		return path.filename() != ".";
	}

	static BundleContents ValidateBundle(const ArtifactBundlePublicationRequest& request)
	{
		BundleContents contents;
		std::unordered_set<std::wstring> paths;
		const auto completionKey = Paths::MakePathKey(request.CompletionFile);
		for (const ArtifactBundleFile& file : request.Files)
		{
			const auto key = Paths::MakePathKey(file.RelativePath);
			if (!IsRelativeFilePath(file.RelativePath) || !paths.insert(key).second)
			{
				throw Diagnostics::Error("Artifact bundle contains an invalid or duplicate file path.");
			}
			if (file.Bytes.size() > (std::numeric_limits<std::uintmax_t>::max)() - contents.Bytes)
			{
				throw Diagnostics::Error("Artifact bundle size overflowed.");
			}
			contents.Bytes += file.Bytes.size();
			if (key == completionKey)
			{
				contents.Completion = &file;
			}
		}
		const bool bIncomplete = request.Files.empty() || contents.Completion == nullptr;
		if (bIncomplete || contents.Bytes > request.MaximumOutputBytes)
		{
			throw Diagnostics::Error("Artifact bundle is incomplete or exceeds its output budget.");
		}
		return contents;
	}

	static void CheckFreeSpace(const std::filesystem::path& root, std::uintmax_t bytes)
	{
		std::error_code error;
		const auto space = std::filesystem::space(root, error);
		// Dividing available space avoids overflow while reserving the 2.1x staging margin.
		const std::uintmax_t supportedBytes = (space.available / 21u) * 10u + ((space.available % 21u) * 10u) / 21u;
		if (error || bytes > supportedBytes)
		{
			throw Diagnostics::Error("Artifact publication does not have enough free space.");
		}
	}

	static std::filesystem::path PreparePublicationDirectory(const ArtifactBundlePublicationRequest& request, std::uintmax_t bytes)
	{
		std::filesystem::create_directories(request.AllowedRoot);
		const auto root = std::filesystem::weakly_canonical(request.AllowedRoot);
		const auto directory = std::filesystem::weakly_canonical(request.PublicationDirectory);
		const bool bContained = directory != root && Paths::IsUnderRoot(directory, root);
		if (!bContained || std::filesystem::exists(directory))
		{
			throw Diagnostics::Error("Artifact publication path is outside its allowed root or already exists.");
		}
		CheckFreeSpace(root, bytes);
		return directory;
	}

	class ArtifactStagingDirectory final
	{
	public:
		explicit ArtifactStagingDirectory(const std::filesystem::path& publication) :
		    m_path(publication)
		{
			m_path += ".staging";
			if (std::filesystem::exists(m_path) || !std::filesystem::create_directories(m_path))
			{
				throw Diagnostics::Error("Artifact staging directory could not be created.");
			}
		}

		~ArtifactStagingDirectory()
		{
			if (!m_bPublished)
			{
				std::error_code error;
				std::filesystem::remove_all(m_path, error);
			}
		}

		ArtifactStagingDirectory(const ArtifactStagingDirectory&) = delete;
		ArtifactStagingDirectory& operator=(const ArtifactStagingDirectory&) = delete;

		const std::filesystem::path& GetPath() const noexcept { return m_path; }

		void Publish(const std::filesystem::path& destination)
		{
			std::filesystem::rename(m_path, destination);
			m_bPublished = true;
		}

	private:
		std::filesystem::path m_path;
		bool m_bPublished = false;
	};

	static ArtifactBundlePublishedFile WriteVerifiedFile(const std::filesystem::path& directory, const ArtifactBundleFile& file)
	{
		const auto path = directory / file.RelativePath;
		Hash::Sha256Digest sourceHash{};
		Hash::Sha256Digest storedHash{};
		std::string error;
		const bool bSourceHashed = Hash::TrySha256(file.Bytes.data(), file.Bytes.size(), sourceHash, error);
		const bool bWritten = bSourceHashed && Files::TryWriteAllBytes(path, file.Bytes, error);
		const bool bStoredHashed = bWritten && Hash::TrySha256File(path, storedHash, error);
		if (!bStoredHashed || sourceHash != storedHash)
		{
			throw Diagnostics::Error(error.empty() ? "Artifact verification failed after writing." : error);
		}
		return {.RelativePath = file.RelativePath, .Size = static_cast<std::uint64_t>(file.Bytes.size()), .Sha256 = Hash::Sha256ToHex(sourceHash)};
	}

	static std::vector<ArtifactBundlePublishedFile> WriteBundleFiles(
	    const ArtifactBundlePublicationRequest& request,
	    const ArtifactBundleFile& completion,
	    const std::filesystem::path& staging,
	    std::stop_token token)
	{
		std::vector<ArtifactBundlePublishedFile> receipts;
		receipts.reserve(request.Files.size());
		for (const ArtifactBundleFile& file : request.Files)
		{
			if (&file != &completion)
			{
				CheckCancellation(token);
				receipts.push_back(WriteVerifiedFile(staging, file));
			}
		}
		CheckCancellation(token);
		receipts.push_back(WriteVerifiedFile(staging, completion));
		return receipts;
	}
}

ArtifactBundlePublicationResult ArtifactBundlePublication::Publish(ArtifactBundlePublicationRequest request, std::stop_token cancellationToken) noexcept
{
	ArtifactBundlePublicationResult result;
	try
	{
		result.PublicationDirectory = request.PublicationDirectory;
		CheckCancellation(cancellationToken);
		const BundleContents contents = ValidateBundle(request);
		const auto directory = PreparePublicationDirectory(request, contents.Bytes);
		ArtifactStagingDirectory staging(directory);
		result.Files = WriteBundleFiles(request, *contents.Completion, staging.GetPath(), cancellationToken);
		result.PublicationDirectory = directory;
		CheckCancellation(cancellationToken);
		staging.Publish(directory);
		result.bSucceeded = true;
	}
	catch (const std::exception& error)
	{
		result.ErrorMessage = error.what();
	}
	catch (...)
	{
		result.ErrorMessage = "Unknown artifact publication failure.";
	}
	return result;
}
