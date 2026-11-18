#include "PCH.h"
#include "Scene/GpuScene/RayTracing/RenderGpuRayTracingPayloadBuilder.h"

#include "Core/Public/Diagnostics/Verify.h"
#include "Meshes/GpuMesh.h"
#include "Meshes/GpuMeshCache.h"
#include "Meshes/GpuMeshPreparation.h"
#include "Scene/Preparation/PreparedRenderScene.h"

#include <algorithm>
#include <limits>
#include <unordered_map>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_renderGpuRayTracingPayloadBuilderLogger, "Renderer.RenderGpuRayTracingPayloadBuilder");

struct RenderGpuMeshHitDataOffsets final
{
	std::uint32_t FirstVertex = 0;
	std::uint32_t FirstIndex = 0;
	std::uint32_t FirstMorphTargetDelta = 0;
	std::uint32_t IndexCount = 0;
};

using MeshHitDataOffsets = std::unordered_map<const GpuMesh*, RenderGpuMeshHitDataOffsets>;

static RenderGpuMeshHitDataOffsets ResolveMeshOffsets(MeshHitDataOffsets& meshOffsets, const GpuMesh& mesh, RenderGpuRayTracingPayloads& payloads)
{
	if (const auto existing = meshOffsets.find(&mesh); existing != meshOffsets.end())
	{
		return existing->second;
	}

	const std::span<const RayTracingHitVertex> meshVertices = mesh.GetRayTracingHitVertices();
	const std::span<const std::uint32_t> meshIndices = mesh.GetRayTracingHitIndices();
	const std::span<const VertexSkinInfluence> skinInfluences = mesh.GetSkinInfluences();
	const std::span<const MorphTargetDeltaData> morphTargetDeltas = mesh.GetMorphTargetDeltas();
	constexpr std::size_t maximumPayloadElementCount = (std::numeric_limits<std::uint32_t>::max)();
	const bool bHitVerticesReady = mesh.HasRayTracingHitData() && !meshVertices.empty();
	const bool bTriangleDataReady = bHitVerticesReady && (meshIndices.size() >= 3u);
	const bool bHitDataReady = bTriangleDataReady && (meshIndices.size() % 3u == 0u);
	const bool bCheckSkinInfluences = bHitDataReady && (mesh.HasSkinInfluences());
	const bool bInfluenceCountMismatch = bCheckSkinInfluences && (skinInfluences.size() != meshVertices.size());
	const bool bSkinningValid = bHitDataReady && !bInfluenceCountMismatch;

	const bool bVertexPayloadBounded = bSkinningValid && payloads.Vertices.size() <= maximumPayloadElementCount;
	const bool bIndexPayloadBounded = bVertexPayloadBounded && (payloads.Indices.size() <= maximumPayloadElementCount);
	const bool bPayloadsBounded = bIndexPayloadBounded && (payloads.MorphTargetDeltas.size() <= maximumPayloadElementCount);

	const bool bVerticesFit = bPayloadsBounded && meshVertices.size() <= maximumPayloadElementCount - payloads.Vertices.size();
	const bool bIndicesFit = bVerticesFit && meshIndices.size() <= maximumPayloadElementCount - payloads.Indices.size();
	const bool bMorphTargetsFit = bIndicesFit && morphTargetDeltas.size() <= maximumPayloadElementCount - payloads.MorphTargetDeltas.size();

	if (!bMorphTargetsFit)
	{
		Diagnostics::Fatal(g_renderGpuRayTracingPayloadBuilderLogger, __FILE__, __LINE__, "Ray-tracing work references invalid mesh hit data.");
	}

	const RenderGpuMeshHitDataOffsets offsets{
	    .FirstVertex = static_cast<std::uint32_t>(payloads.Vertices.size()),
	    .FirstIndex = static_cast<std::uint32_t>(payloads.Indices.size()),
	    .FirstMorphTargetDelta = static_cast<std::uint32_t>(payloads.MorphTargetDeltas.size()),
	    .IndexCount = static_cast<std::uint32_t>(meshIndices.size())};

	payloads.Vertices.insert(payloads.Vertices.end(), meshVertices.begin(), meshVertices.end());
	if (mesh.HasSkinInfluences())
	{
		for (const VertexSkinInfluence& influence : skinInfluences)
		{
			payloads.SkinInfluences.push_back(ConvertGpuMeshSkinInfluence(influence));
		}
	}
	else
	{
		payloads.SkinInfluences.resize(payloads.Vertices.size());
	}
	payloads.Indices.insert(payloads.Indices.end(), meshIndices.begin(), meshIndices.end());
	payloads.MorphTargetDeltas.insert(payloads.MorphTargetDeltas.end(), morphTargetDeltas.begin(), morphTargetDeltas.end());
	meshOffsets.emplace(&mesh, offsets);
	return offsets;
}

static void Clear(RenderGpuRayTracingPayloads& payloads) noexcept
{
	payloads.Vertices.clear();
	payloads.SkinInfluences.clear();
	payloads.MorphTargetDeltas.clear();
	payloads.Indices.clear();
	payloads.Instances.clear();
	payloads.Materials.clear();
	payloads.InstanceCount = 0u;
	payloads.MaterialCount = 0u;
}

static void AppendMaterials(const PreparedRenderScene& preparedScene, RenderGpuRayTracingPayloads& payloads)
{
	payloads.Materials.reserve(preparedScene.materials.size());
	for (const MaterialData& material : preparedScene.materials)
	{
		const bool bEmissiveRedOrGreen = material.emissiveColor.x > 0.0f || material.emissiveColor.y > 0.0f;
		const bool bEmissive = bEmissiveRedOrGreen || material.emissiveColor.z > 0.0f;

		RayTracingHitMaterial rayTracingMaterial{
		    .BaseColor = material.baseColor,
		    .EmissiveColor = material.emissiveColor,
		    .Metallic = material.metallic,
		    .Roughness = material.roughness,
		    .F0 = material.f0,
		    .AlphaCutoff = material.alphaCutoff,
		    .AlphaMode = material.alphaMode,
		    .TextureFlags = material.textureFlags,
		    .SubsurfaceColor = material.subsurfaceColor,
		    .SubsurfaceStrength = material.subsurfaceStrength,
		    .Flags = bEmissive ? RayTracingHitData::MaterialFlagEmissive : 0u,
		    .TextureIndices0 =
		        DirectX::XMUINT4{
		            material.materialTextureIndices[MaterialTextureSlots::BaseColor],
		            material.materialTextureIndices[MaterialTextureSlots::Normal],
		            material.materialTextureIndices[MaterialTextureSlots::Roughness],
		            material.materialTextureIndices[MaterialTextureSlots::Metallic]},
		    .TextureIndices1 =
		        DirectX::XMUINT4{
		            material.materialTextureIndices[MaterialTextureSlots::Occlusion],
		            material.materialTextureIndices[MaterialTextureSlots::Emissive],
		            material.materialTextureIndices[MaterialTextureSlots::SubsurfaceColor],
		            material.materialTextureIndices[MaterialTextureSlots::SubsurfaceStrength]},
		    .TextureMappings = material.materialTextureMappings};

		payloads.Materials.push_back(rayTracingMaterial);
	}
}

static void PrepareInstances(const PreparedRenderScene& preparedScene, RenderGpuRayTracingPayloads& payloads, MeshHitDataOffsets& meshOffsets)
{
	std::uint32_t instanceCapacity = 1;
	for (const RenderRayTracingInstance& input : preparedScene.rayTracingInstances)
	{
		instanceCapacity = std::max(instanceCapacity, input.GpuSceneSlot + 1u);
	}

	payloads.Instances.resize(instanceCapacity);
	meshOffsets.reserve(preparedScene.rayTracingInstances.size());
}

static void AppendInstance(
    const RenderRayTracingInstance& input,
    const PreparedRenderScene& preparedScene,
    const GpuMeshCache& meshes,
    RenderGpuRayTracingPayloads& payloads,
    MeshHitDataOffsets& meshOffsets)
{
	if (input.PrimitiveIndex >= preparedScene.primitives.size())
	{
		Diagnostics::Fatal(g_renderGpuRayTracingPayloadBuilderLogger, __FILE__, __LINE__, "Ray-tracing work references a mesh instance outside the render scene.");
	}

	const MeshDraw& draw = preparedScene.primitives[input.PrimitiveIndex].Draw;
	if (draw.MaterialSlot >= preparedScene.materials.size())
	{
		Diagnostics::Fatal(g_renderGpuRayTracingPayloadBuilderLogger, __FILE__, __LINE__, "Ray-tracing work references a material outside the render scene.");
	}

	const GpuMesh* gpuMesh = meshes.Resolve(draw.Geometry.Mesh);

	const bool bSkeletal = draw.Geometry.MeshKind == RenderMeshKind::Skeletal;
	const bool bJointOffsetMissing = bSkeletal && draw.Skinning.JointMatrixOffset == kInvalidMeshInstanceJointMatrixOffset;
	const bool bCheckMesh = bSkeletal && !bJointOffsetMissing;
	const bool bMeshMissing = bCheckMesh && gpuMesh == nullptr;
	const bool bStorageMissing = bJointOffsetMissing || bMeshMissing;
	const bool bCheckInfluences = bSkeletal && !bStorageMissing;
	const bool bInfluencesMissing = bCheckInfluences && !gpuMesh->HasSkinInfluences();
	const bool bMissingSkinning = bStorageMissing || bInfluencesMissing;

	if (gpuMesh == nullptr || bMissingSkinning)
	{
		Diagnostics::Fatal(g_renderGpuRayTracingPayloadBuilderLogger, __FILE__, __LINE__, "Ray-tracing work references incomplete mesh or skinning data.");
	}

	const MaterialData& material = preparedScene.materials[draw.MaterialSlot];
	const RenderGpuMeshHitDataOffsets offsets = ResolveMeshOffsets(meshOffsets, *gpuMesh, payloads);

	payloads.Instances[input.GpuSceneSlot] = RayTracingHitInstance{
	    .FirstVertex = offsets.FirstVertex,
	    .FirstIndex = offsets.FirstIndex,
	    .IndexCount = offsets.IndexCount,
	    .MaterialSlot = draw.MaterialSlot,
	    .Flags = RayTracingHitData::InstanceFlagValid | (material.bDoubleSided ? RayTracingHitData::InstanceFlagTwoSided : 0u),
	    .MorphTargetDeltaOffset = offsets.FirstMorphTargetDelta};
}

void BuildRenderGpuRayTracingPayloads(const PreparedRenderScene& preparedScene, const GpuMeshCache& meshes, RenderGpuRayTracingPayloads& payloads)
{
	Clear(payloads);

	const auto& instances = preparedScene.rayTracingInstances;
	if (instances.empty())
	{
		return;
	}
	if (preparedScene.materials.empty())
	{
		Diagnostics::Fatal(g_renderGpuRayTracingPayloadBuilderLogger, __FILE__, __LINE__, "Ray-tracing work exists without any material records.");
	}

	AppendMaterials(preparedScene, payloads);

	MeshHitDataOffsets meshOffsets;
	PrepareInstances(preparedScene, payloads, meshOffsets);
	for (const RenderRayTracingInstance& input : instances)
	{
		AppendInstance(input, preparedScene, meshes, payloads, meshOffsets);
	}

	if (payloads.Vertices.empty() || payloads.Indices.empty())
	{
		Diagnostics::Fatal(g_renderGpuRayTracingPayloadBuilderLogger, __FILE__, __LINE__, "Ray-tracing work produced no geometry payload.");
	}

	payloads.InstanceCount = static_cast<std::uint32_t>(payloads.Instances.size());
	payloads.MaterialCount = static_cast<std::uint32_t>(payloads.Materials.size());
}
