#include "PCH.h"

#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactPublication.h"

#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactEncoding.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Hash/FileHash.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Paths/PathUtils.h"

#include <array>
#include <filesystem>
#include <limits>
#include <span>
#include <utility>

namespace ReferencePathTracerArtifactPublication
{
	static void CheckCancellation(std::stop_token token)
	{
		if (token.stop_requested())
		{
			throw Diagnostics::Error("Artifact publication was cancelled.");
		}
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

	static std::filesystem::path NativeArtifactPath(const std::filesystem::path& path)
	{
		const auto absolutePath = std::filesystem::absolute(path);
		std::wstring nativePath = absolutePath.lexically_normal().native();
		if (!nativePath.starts_with(L"\\\\?\\"))
		{
			const bool bUncPath = nativePath.starts_with(L"\\\\");
			nativePath = bUncPath ? L"\\\\?\\UNC\\" + nativePath.substr(2) : L"\\\\?\\" + nativePath;
		}
		return std::filesystem::path(std::move(nativePath));
	}

	static std::filesystem::path PreparePublicationDirectory(const ReferencePathTracerArtifactWriteRequest& request, std::uintmax_t bytes)
	{
		const auto nativeRoot = NativeArtifactPath(request.AllowedRoot);
		const auto root = std::filesystem::weakly_canonical(nativeRoot);
		std::filesystem::create_directories(root);
		const auto nativeDirectory = NativeArtifactPath(request.PublicationDirectory);
		const auto directory = std::filesystem::weakly_canonical(nativeDirectory);
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
		explicit ArtifactStagingDirectory(const std::filesystem::path& publication);

		~ArtifactStagingDirectory();

		ArtifactStagingDirectory(const ArtifactStagingDirectory&) = delete;
		ArtifactStagingDirectory& operator=(const ArtifactStagingDirectory&) = delete;

		const std::filesystem::path& GetPath() const noexcept { return m_path; }

		void Publish(const std::filesystem::path& destination);

	private:
		std::filesystem::path m_path;
		bool m_bPublished = false;
	};

	ArtifactStagingDirectory::ArtifactStagingDirectory(const std::filesystem::path& publication) :
	    m_path(publication)
	{
		m_path += ".staging";
		if (std::filesystem::exists(m_path) || !std::filesystem::create_directories(m_path))
		{
			throw Diagnostics::Error("Artifact staging directory could not be created.");
		}
	}

	ArtifactStagingDirectory::~ArtifactStagingDirectory()
	{
		if (!m_bPublished)
		{
			std::error_code error;
			std::filesystem::remove_all(m_path, error);
		}
	}

	void ArtifactStagingDirectory::Publish(const std::filesystem::path& destination)
	{
		std::filesystem::rename(m_path, destination);
		m_bPublished = true;
	}

	static std::uintmax_t CountOutputBytes(const ReferencePathTracerEncodedArtifact& artifact, std::string_view manifest, std::uint64_t maximumBytes)
	{
		const std::array sizes{artifact.Beauty.size(), artifact.Checkpoint ? artifact.Checkpoint->size() : 0u, manifest.size()};
		std::uintmax_t total = 0;
		for (const std::size_t size : sizes)
		{
			if (size > (std::numeric_limits<std::uintmax_t>::max)() - total)
			{
				throw Diagnostics::Error("Artifact bundle size overflowed.");
			}
			total += size;
		}
		if (total > maximumBytes)
		{
			throw Diagnostics::Error("Artifact bundle is incomplete or exceeds its output budget.");
		}
		return total;
	}

	static void WriteVerifiedFile(const std::filesystem::path& path, std::span<const std::byte> bytes, std::string_view expectedHash)
	{
		Hash::Sha256Digest storedHash{};
		std::string error;
		const bool bWritten = Files::TryWriteAllBytes(path, bytes, error);
		const bool bStoredHashed = bWritten && Hash::TrySha256File(path, storedHash, error);
		const bool bVerified = bStoredHashed && Hash::Sha256ToHex(storedHash) == expectedHash;
		if (!bVerified)
		{
			throw Diagnostics::Error(error.empty() ? "Artifact verification failed after writing." : error);
		}
	}

	std::string Publish(
	    const ReferencePathTracerArtifactWriteRequest& request,
	    const ReferencePathTracerEncodedArtifact& artifact,
	    std::string_view manifest,
	    std::string_view beautyHash,
	    std::string_view checkpointHash,
	    std::stop_token cancellationToken)
	{
		CheckCancellation(cancellationToken);
		const auto bytes = CountOutputBytes(artifact, manifest, request.MaximumOutputBytes);
		const auto directory = PreparePublicationDirectory(request, bytes);
		ArtifactStagingDirectory staging(directory);
		CheckCancellation(cancellationToken);
		WriteVerifiedFile(staging.GetPath() / "beauty.exr", artifact.Beauty, beautyHash);
		if (artifact.Checkpoint)
		{
			CheckCancellation(cancellationToken);
			WriteVerifiedFile(staging.GetPath() / "checkpoint.bin", *artifact.Checkpoint, checkpointHash);
		}
		CheckCancellation(cancellationToken);
		std::string manifestHash;
		std::string error;
		if (!Hash::TrySha256Hex(manifest, manifestHash, error))
		{
			throw Diagnostics::Error(error);
		}
		const auto manifestBytes = std::as_bytes(std::span(manifest));
		WriteVerifiedFile(staging.GetPath() / "manifest.json", manifestBytes, manifestHash);
		CheckCancellation(cancellationToken);
		staging.Publish(directory);
		return manifestHash;
	}
}
