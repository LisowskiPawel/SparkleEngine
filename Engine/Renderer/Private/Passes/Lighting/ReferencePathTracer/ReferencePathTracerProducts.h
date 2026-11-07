#pragma once

struct ReferencePathTracerGraphResources;
struct RenderFrameGraphResources;

void PublishReferencePathTracerProducts(const ReferencePathTracerGraphResources& graphResources, bool bPublishRadiance, RenderFrameGraphResources& resources);
