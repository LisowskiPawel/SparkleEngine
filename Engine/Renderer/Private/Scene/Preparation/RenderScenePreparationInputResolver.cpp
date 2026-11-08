#include "PCH.h"

#include "Scene/Preparation/RenderScenePreparationInputResolver.h"

#include "Core/Public/Diagnostics/Verify.h"
#include "Meshes/GpuMesh.h"
#include "Meshes/GpuMeshCache.h"
#include "Passes/Lighting/Sky/Sky.h"
#include "Scene/Materials/MaterialHandleResolver.h"
#include "Scene/Geometry/MeshRenderingControls.h"
#include "Scene/Preparation/RenderScenePreparationRun.h"
#include "Scene/Preparation/Geometry/RenderMeshClassificationConversion.h"
#include "Scene/RenderScene.h"
#include "ShaderData/MeshInstanceShaderData.h"
#include "Textures/RendererTexture.h"
#include "Textures/TextureCache.h"

#include <cstdint>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_renderPreparationInputResolverLogger, "Renderer.RenderScenePreparationInputResolver");

static ResolvedRenderPrimitive ResolvePrimitive(
    const RenderScene& scene,
    const GpuMeshCache& gpuMeshCache,
    const RenderPrimitive& primitive,
    std::uint32_t materialGeneration,
    PreparedRenderScene& preparedScene)
{
	if (!primitive.Dynamic.Object.IsValid() || !primitive.Static.Mesh.IsValid())
	{
		Diagnostics::Fatal(g_renderPreparationInputResolverLogger, __FILE__, __LINE__, "Render scene primitive contains an invalid primitive or mesh identity.");
	}

	const GpuMesh* gpuMesh = gpuMeshCache.Resolve(primitive.GpuMesh);
	if (gpuMesh == nullptr || !gpuMesh->IsValid())
	{
		Diagnostics::Fatal(g_renderPreparationInputResolverLogger, __FILE__, __LINE__, "Resident render scene primitive has no GPU mesh.");
	}

	const std::uint32_t materialSlot = MaterialHandleResolver::ResolveSlot(primitive.Static.Material, materialGeneration, preparedScene.materials.size());
	const MaterialData& material = preparedScene.materials[materialSlot];

	MeshDraw draw;
	draw.MaterialSlot = materialSlot;
	draw.Source.GpuSceneSlot = primitive.GpuSceneSlot;
	draw.Source.MeshAssetId = primitive.Static.Mesh.GetAssetId();
	draw.Source.MeshGeneration = primitive.Static.Mesh.GetGeneration();
	draw.Skinning.SkeletonAssetId = primitive.Static.Skeleton.GetAssetId();
	draw.Skinning.JointMatrixOffset = kInvalidMeshInstanceJointMatrixOffset;
	draw.Geometry.MeshKind = RenderMeshClassificationConversion::ToRenderMeshKind(primitive.Static.MeshKind);
	draw.Geometry.Mesh = gpuMesh->GetHandle();
	draw.Geometry.LocalBoundsMin = gpuMesh->GetLocalBounds().Min;
	draw.Geometry.LocalBoundsMax = gpuMesh->GetLocalBounds().Max;
	draw.Geometry.bHasLocalBounds = gpuMesh->GetLocalBounds().bValid;

	return ResolvedRenderPrimitive{
	    .Object = primitive.Object,
	    .Draw = draw,
	    .WorldMatrix = primitive.Dynamic.WorldMatrix,
	    .PreviousWorldMatrix = scene.ResolvePreviousWorldMatrix(primitive),
	    .WorldInverseTranspose = primitive.Dynamic.WorldInverseTranspose,
	    .Material = material.gpuHandle,
	    .InstanceGroupIndex = RenderMeshClassificationConversion::ToRenderMeshInstanceGroupIndex(primitive.Static.InstanceGroupIndex),
	    .MaterialAlphaMode = material.alphaMode,
	    .MorphTargetCount = gpuMesh->GetMorphTargetCount(),
	    .MorphTargetVertexCount = gpuMesh->GetVertexCount()};
}

static void ResolvePrimitives(const RenderScene& scene, const GpuMeshCache& gpuMeshCache, RenderScenePreparationRun& run)
{
	run.ResolvedPrimitives.clear();
	run.ResolvedPrimitives.reserve(scene.GetPrimitives().size());
	for (const RenderPrimitive& primitive : scene.GetPrimitives())
	{
		if (!primitive.Dynamic.bVisible || !primitive.bGpuMeshResident || !IsMeshRenderingEnabled(primitive.Static.MeshKind))
		{
			continue;
		}
		run.ResolvedPrimitives.push_back(ResolvePrimitive(scene, gpuMeshCache, primitive, scene.GetMaterials().Generation, run.PreparedScene));
	}
}

static void ResolveInstanceGroups(const RenderScene& scene, RenderScenePreparationRun& run)
{
	run.PreparedScene.instanceGroups.clear();
	run.PreparedScene.instanceGroups.reserve(scene.GetInstanceGroups().size());
	for (const RenderMeshInstanceGroupData& group : scene.GetInstanceGroups())
	{
		run.PreparedScene.instanceGroups.push_back(
		    RenderMeshInstanceGroup{.groupKind = RenderMeshClassificationConversion::ToRenderMeshInstanceGroupKind(group.Kind), .instanceCount = group.InstanceCount});
	}
}

static void ResolveSky(const RenderScene& scene, const TextureCache& textureCache, PreparedRenderScene& preparedScene)
{
	const RendererTexture* skyTexture = nullptr;
	const SceneSkyDesc* sky = scene.GetSky() ? &*scene.GetSky() : nullptr;
	if (sky == nullptr)
	{
		skyTexture = textureCache.ResolveDefaultSkyTexture();
	}
	else
	{
		preparedScene.sky.bEnabled = sky->bEnabled;
		preparedScene.sky.color = sky->color;
		preparedScene.sky.brightness = sky->brightness;
		if (!sky->skyTexture.IsValid())
		{
			skyTexture = textureCache.ResolveDefaultSkyTexture();
		}
		else
		{
			skyTexture = textureCache.GetSceneTexture(sky->skyTexture.texturePath);
		}
	}
	if (skyTexture == nullptr || !*skyTexture)
	{
		Diagnostics::Fatal(g_renderPreparationInputResolverLogger, __FILE__, __LINE__, "Scene sky texture is unavailable.");
	}
	preparedScene.sky.texture = skyTexture;
	preparedScene.sky.bEnabled = preparedScene.sky.bEnabled && CVarSkyEnabled.Get();
}

static void CaptureSceneInputs(RenderScene& scene, RenderScenePreparationRun& run)
{
	run.Lights = scene.GetLights();
	run.PreparedScene.structuralRevision = scene.GetStructuralRevision();
	run.PreparedScene.materialRevision = scene.GetMaterialRevision();
	scene.BuildMaterials(run.PreparedScene);
}

static void SizePreparationOutputs(const RenderScene& scene, RenderScenePreparationRun& run)
{
	run.PreparedPrimitives.resize(run.ResolvedPrimitives.size());
	run.PreparedLights.resize(scene.GetLights().size());
}

void ResolveRenderScenePreparationInputs(RenderScene& scene, const GpuMeshCache& gpuMeshCache, const TextureCache& textureCache, RenderScenePreparationRun& run)
{
	CaptureSceneInputs(scene, run);
	ResolveSky(scene, textureCache, run.PreparedScene);
	ResolvePrimitives(scene, gpuMeshCache, run);
	ResolveInstanceGroups(scene, run);
	SizePreparationOutputs(scene, run);
}
