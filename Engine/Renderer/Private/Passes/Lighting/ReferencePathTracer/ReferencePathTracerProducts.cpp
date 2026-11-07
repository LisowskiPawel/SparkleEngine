#include "PCH.h"
#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerProducts.h"

#include "Frame/Graph/RenderFrameGraphResources.h"
#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerResources.h"

void PublishReferencePathTracerProducts(const ReferencePathTracerGraphResources& graphResources, bool bPublishRadiance, RenderFrameGraphResources& resources)
{
	resources.ViewportProducts.Radiance = bPublishRadiance ? graphResources.CommittedMean : FrameGraphTextureHandle::Invalid();
	resources.ViewportProducts.RadianceSecondMoment = bPublishRadiance ? graphResources.CommittedM2 : FrameGraphTextureHandle::Invalid();
	resources.ViewportProducts.SceneDepth = FrameGraphTextureHandle::Invalid();
}
