#include "PCH.h"
#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerProducts.h"

#include "Frame/Graph/RenderFrameGraphResources.h"
#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerResources.h"

void PublishReferencePathTracerProducts(const ReferencePathTracerGraphResources& graphResources, bool publishRadiance, RenderFrameGraphResources& resources)
{
	resources.ViewportProducts.Radiance = publishRadiance ? graphResources.CommittedMean : FrameGraphTextureHandle::Invalid();
	resources.ViewportProducts.RadianceSecondMoment = publishRadiance ? graphResources.CommittedM2 : FrameGraphTextureHandle::Invalid();
	resources.ViewportProducts.SceneDepth = FrameGraphTextureHandle::Invalid();
}
