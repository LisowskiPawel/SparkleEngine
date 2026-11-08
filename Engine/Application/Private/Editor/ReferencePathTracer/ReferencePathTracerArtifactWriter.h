#pragma once

#include "Editor/ReferencePathTracer/ReferencePathTracerArtifact.h"

#include <stop_token>

namespace ReferencePathTracerArtifactWriter
{
	ReferencePathTracerArtifactWriteResult Write(ReferencePathTracerArtifactWriteRequest request, std::stop_token cancellationToken) noexcept;
}
