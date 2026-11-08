#include "PCH.h"

#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactWriter.h"

#include "Core/Public/Hash/HashUtils.h"
#include "Editor/Artifacts/ArtifactBundlePublication.h"
#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactEncoding.h"

#include <utility>
#include <exception>

namespace ReferencePathTracerArtifactWriter
{
	static bool HashArtifact(const ReferencePathTracerEncodedArtifact& artifact, std::string& beauty, std::string& checkpoint, std::string& error)
	{
		if (!Hash::TrySha256Hex(artifact.Beauty, beauty, error))
		{
			return false;
		}
		return !artifact.Checkpoint || Hash::TrySha256Hex(*artifact.Checkpoint, checkpoint, error);
	}

	static ArtifactBundlePublicationRequest BuildPublication(ReferencePathTracerArtifactWriteRequest& request, ReferencePathTracerEncodedArtifact& artifact, const std::string& manifest)
	{
		ArtifactBundlePublicationRequest publication{
		    .AllowedRoot = std::move(request.AllowedRoot),
		    .PublicationDirectory = std::move(request.PublicationDirectory),
		    .CompletionFile = "manifest.json",
		    .MaximumOutputBytes = request.MaximumOutputBytes};

		publication.Files.push_back(ArtifactBundleFile{.RelativePath = "beauty.exr", .Bytes = std::move(artifact.Beauty)});
		if (artifact.Checkpoint)
		{
			publication.Files.push_back(ArtifactBundleFile{.RelativePath = "checkpoint.bin", .Bytes = std::move(*artifact.Checkpoint)});
		}

		publication.Files.push_back(
		    ArtifactBundleFile{
		        .RelativePath = "manifest.json",
		        .Bytes = std::vector<std::byte>(reinterpret_cast<const std::byte*>(manifest.data()), reinterpret_cast<const std::byte*>(manifest.data() + manifest.size()))});

		return publication;
	}
}

ReferencePathTracerArtifactWriteResult ReferencePathTracerArtifactWriter::Write(ReferencePathTracerArtifactWriteRequest request, std::stop_token cancellationToken) noexcept
{
	ReferencePathTracerArtifactWriteResult result;
	try
	{
		result.PublicationDirectory = request.PublicationDirectory;
		if (cancellationToken.stop_requested())
		{
			result.ErrorMessage = "Reference Path Tracer artifact publication was cancelled.";
			return result;
		}
		ReferencePathTracerEncodedArtifact artifact;
		if (!ReferencePathTracerArtifactEncoding::Encode(request, artifact, result.ErrorMessage))
		{
			return result;
		}
		std::string beautyHash;
		std::string checkpointHash;
		if (!HashArtifact(artifact, beautyHash, checkpointHash, result.ErrorMessage))
		{
			return result;
		}
		const std::string manifest = ReferencePathTracerArtifactEncoding::BuildManifest(request, artifact, beautyHash, checkpointHash);
		ArtifactBundlePublicationRequest publication = BuildPublication(request, artifact, manifest);
		ArtifactBundlePublicationResult published = ArtifactBundlePublication::Publish(std::move(publication), cancellationToken);
		result.bSucceeded = published.bSucceeded;
		result.PublicationDirectory = std::move(published.PublicationDirectory);
		result.ErrorMessage = std::move(published.ErrorMessage);
		if (result.bSucceeded && !published.Files.empty())
		{
			result.ManifestSha256 = std::move(published.Files.back().Sha256);
		}
	}
	catch (const std::exception& error)
	{
		result.ErrorMessage = error.what();
	}
	catch (...)
	{
		result.ErrorMessage = "Unknown Reference Path Tracer artifact failure.";
	}
	return result;
}
