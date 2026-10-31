#pragma once

struct ReferencePathTracerGraphResources;
struct RenderFrameGraphResources;

void PublishReferencePathTracerProducts(const ReferencePathTracerGraphResources& graphResources, bool publishRadiance, RenderFrameGraphResources& resources);
