#include "PCH.h"

#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactWriter.h"

#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactEncoding.h"
#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactPublication.h"
#include "Core/Public/Hash/HashUtils.h"

#include <exception>
#include <string>

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
		result.ManifestSha256 = ReferencePathTracerArtifactPublication::Publish(request, artifact, manifest, beautyHash, checkpointHash, cancellationToken);
		result.bSucceeded = true;
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
