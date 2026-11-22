#pragma once

#include <memory>

class GpuMeshCache;
class RenderScene;
struct enkiTaskScheduler;
class TextureCache;
struct PreparedRenderScene;

class RenderScenePreparation final
{
public:
	RenderScenePreparation(enkiTaskScheduler* scheduler, GpuMeshCache& gpuMeshCache, TextureCache& textureCache);
	~RenderScenePreparation() noexcept;

	RenderScenePreparation(const RenderScenePreparation&) = delete;
	RenderScenePreparation& operator=(const RenderScenePreparation&) = delete;
	RenderScenePreparation(RenderScenePreparation&&) = delete;
	RenderScenePreparation& operator=(RenderScenePreparation&&) = delete;

	void Execute(RenderScene& scene, PreparedRenderScene& output);

private:
	struct Impl;
	std::unique_ptr<Impl> m_impl;
};
