#pragma once

class GpuMeshCache;
class RenderScene;
class TextureCache;
struct RenderScenePreparationRun;

void ResolveRenderScenePreparationInputs(RenderScene& scene, const GpuMeshCache& gpuMeshCache, const TextureCache& textureCache, RenderScenePreparationRun& run);
