#include "PCH.h"
#include "Scene/GpuScene/RenderGpuScene.h"

#include "RHI/Public/Frame/RhiFrameConstants.h"
#include "Scene/GpuScene/RenderGpuGeometryBuffers.h"
#include "Scene/GpuScene/RenderGpuGeometryState.h"
#include "Scene/GpuScene/RenderGpuLighting.h"
#include "Scene/GpuScene/RenderGpuRayTracing.h"

#include <array>

struct RenderGpuScene::Impl final
{
	Impl(RhiResourceService& resourceService, const GpuMeshCache& meshes) noexcept :
	    Lighting(resourceService),
	    GeometryBuffers(resourceService),
	    RayTracing(resourceService, meshes)
	{
	}

	const RenderSceneGpuBindings& Update(const PreparedRenderScene& preparedScene, const RenderView& view, std::uint32_t frameIndex)
	{
		RenderSceneGpuBindings& bindings = Frames[frameIndex % Frames.size()];
		Geometry.Update(preparedScene, view);
		bindings.Lighting = Lighting.Update(preparedScene, frameIndex);
		bindings.Geometry = GeometryBuffers.Update(Geometry, frameIndex);
		bindings.RayTracing = RayTracing.Update(preparedScene, frameIndex);
		return bindings;
	}

	void Reset() noexcept
	{
		Lighting.Reset();
		GeometryBuffers.Reset();
		RayTracing.Reset();
		Geometry.Reset();
		Frames = {};
	}

	RenderGpuGeometryState Geometry;
	RenderGpuLighting Lighting;
	RenderGpuGeometryBuffers GeometryBuffers;
	RenderGpuRayTracing RayTracing;
	// Slot reuse waits for its submission token, preserving borrowed bindings through execution.
	std::array<RenderSceneGpuBindings, RhiFrameConstants::MaxFrameSlotCount> Frames;
};

RenderGpuScene::RenderGpuScene(RhiResourceService& resourceService, const GpuMeshCache& meshes) :
    m_impl(std::make_unique<Impl>(resourceService, meshes))
{
}

RenderGpuScene::~RenderGpuScene() noexcept = default;

const RenderSceneGpuBindings& RenderGpuScene::Update(const PreparedRenderScene& preparedScene, const RenderView& view, std::uint32_t frameIndex)
{
	return m_impl->Update(preparedScene, view, frameIndex);
}

void RenderGpuScene::Reset() noexcept
{
	m_impl->Reset();
}
