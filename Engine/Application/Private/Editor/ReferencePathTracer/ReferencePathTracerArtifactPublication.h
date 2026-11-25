#pragma once

#include <stop_token>
#include <string>
#include <string_view>

struct ReferencePathTracerArtifactWriteRequest;
struct ReferencePathTracerEncodedArtifact;

namespace ReferencePathTracerArtifactPublication
{
	std::string Publish(
	    const ReferencePathTracerArtifactWriteRequest& request,
	    const ReferencePathTracerEncodedArtifact& artifact,
	    std::string_view manifest,
	    std::string_view beautyHash,
	    std::string_view checkpointHash,
	    std::stop_token cancellationToken);
}
