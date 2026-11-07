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
#include <unordered_set>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_renderGpuRayTracingPayloadBuilderLogger, "Renderer.RenderGpuRayTracingPayloadBuilder");

struct RenderGpuMeshHitDataOffsets final
{
	std::uint32_t FirstVertex = 0;
	std::uint32_t FirstIndex = 0;
	std::uint32_t FirstMorphTargetDelta = 0;
	std::uint32_t VertexCount = 0;
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
	const bool bMeshDataValidmeshVerticesEmpty = mesh.HasRayTracingHitData() && !meshVertices.empty();
	const bool bMeshDataValidmeshIndicesSize = bMeshDataValidmeshVerticesEmpty && (meshIndices.size() >= 3u);
	const bool bMeshHitDataValid = bMeshDataValidmeshIndicesSize && (meshIndices.size() % 3u == 0u);
	const bool bSkinInfluencesValidSkinInfluences = bMeshHitDataValid && (mesh.HasSkinInfluences());
	const bool bInfluencesValidmeshVerticesSize = bSkinInfluencesValidSkinInfluences && (skinInfluences.size() != meshVertices.size());
	const bool bInfluencesValidmeshVerticesSize2 = bMeshHitDataValid && !bInfluencesValidmeshVerticesSize;
	const bool bSkinInfluencesValid = bMeshHitDataValid && bInfluencesValidmeshVerticesSize2;

	const bool bPayloadSizesValidVertices = bSkinInfluencesValid && payloads.Vertices.size() <= maximumPayloadElementCount;
	const bool bPayloadSizesValidIndices = bPayloadSizesValidVertices && (payloads.Indices.size() <= maximumPayloadElementCount);
	const bool bPayloadSizesValid = bPayloadSizesValidIndices && (payloads.MorphTargetDeltas.size() <= maximumPayloadElementCount);

	const bool bVerticesFit = bPayloadSizesValid && meshVertices.size() <= maximumPayloadElementCount - payloads.Vertices.size();
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
	    .VertexCount = static_cast<std::uint32_t>(meshVertices.size()),
	    .IndexCount = static_cast<std::uint32_t>(meshIndices.size())};

	payloads.Vertices.insert(payloads.Vertices.end(), meshVertices.begin(), meshVertices.end());
	if (mesh.HasSkinInfluences())
	{
		for (const VertexSkinInfluence& influence : skinInfluences)
		{
			payloads.SkinInfluences.push_back(GpuMeshPreparation::ConvertSkinInfluence(influence));
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

static std::uint32_t BuildMaterialFlags(const MaterialData& material) noexcept
{
	std::uint32_t flags = material.bDoubleSided ? RayTracingHitData::MaterialFlag_DoubleSided : 0u;
	if (material.alphaMode == 0)
	{
		flags |= RayTracingHitData::MaterialFlag_Opaque;
	}
	else if (material.alphaMode == 1)
	{
		flags |= RayTracingHitData::MaterialFlag_AlphaTested;
	}
	else if (material.alphaMode == 2)
	{
		flags |= RayTracingHitData::MaterialFlag_AlphaBlended;
	}
	if (material.textureFlags != 0)
	{
		flags |= RayTracingHitData::MaterialFlag_Textured;
	}
	if (material.emissiveColor.x > 0.0f || material.emissiveColor.y > 0.0f || material.emissiveColor.z > 0.0f)
	{
		flags |= RayTracingHitData::MaterialFlag_Emissive;
	}
	return flags;
}

static std::uint32_t BuildGeometryFlags(const MeshDraw& draw, const MaterialData& material) noexcept
{
	std::uint32_t flags = draw.Geometry.MeshKind == RenderMeshKind::Skeletal ? RayTracingHitData::GeometryFlag_SkinnedMesh : RayTracingHitData::GeometryFlag_StaticMesh;
	if (material.alphaMode == 1)
	{
		flags |= RayTracingHitData::GeometryFlag_AlphaTested;
	}
	else if (material.alphaMode == 2)
	{
		flags |= RayTracingHitData::GeometryFlag_AlphaBlended;
	}
	if (material.textureFlags != 0)
	{
		flags |= RayTracingHitData::GeometryFlag_TexturedMaterial;
	}
	if (material.bDoubleSided)
	{
		flags |= RayTracingHitData::GeometryFlag_DoubleSided;
	}
	return flags;
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
		    .Flags = BuildMaterialFlags(material),
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
	std::unordered_set<std::uint32_t> occupiedGpuSceneSlots;
	for (const RenderRayTracingInstance& input : preparedScene.rayTracingInstances)
	{
		const bool bGpuSceneSlotValid = input.GpuSceneSlot != (std::numeric_limits<std::uint32_t>::max)();
		const bool bGpuSceneSlotUnique = bGpuSceneSlotValid && occupiedGpuSceneSlots.insert(input.GpuSceneSlot).second;
		
		if (!bGpuSceneSlotUnique)
		{
			Diagnostics::Fatal(g_renderGpuRayTracingPayloadBuilderLogger, __FILE__, __LINE__, "Ray-tracing work contains an invalid or duplicate GPU-scene slot.");
		}
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

	const bool bMissingSkinningGeometry = draw.Geometry.MeshKind == RenderMeshKind::Skeletal;
	const bool bMissingSkinningSkinning = bMissingSkinningGeometry && (draw.Skinning.JointMatrixOffset == kInvalidMeshInstanceJointMatrixOffset);
	const bool bMissingSkinningGuardgpuMesh = bMissingSkinningGeometry && !bMissingSkinningSkinning;
	const bool bMissingSkinninggpuMesh = bMissingSkinningGuardgpuMesh && (gpuMesh == nullptr);
	const bool bMissingSkinninggpuMesh2 = bMissingSkinningSkinning || bMissingSkinninggpuMesh;
	const bool bMissingSkinningGuardInfluences = bMissingSkinningGeometry && !bMissingSkinninggpuMesh2;
	const bool bMissingSkinningSkinInfluences = bMissingSkinningGuardInfluences && (!gpuMesh->HasSkinInfluences());
	const bool bMissingSkinningSkinInfluences2 = bMissingSkinninggpuMesh2 || bMissingSkinningSkinInfluences;
	const bool bMissingSkinning = bMissingSkinningGeometry && bMissingSkinningSkinInfluences2;

	if (gpuMesh == nullptr || bMissingSkinning)
	{
		Diagnostics::Fatal(g_renderGpuRayTracingPayloadBuilderLogger, __FILE__, __LINE__, "Ray-tracing work references incomplete mesh or skinning data.");
	}

	const MaterialData& material = preparedScene.materials[draw.MaterialSlot];
	const RenderGpuMeshHitDataOffsets offsets = ResolveMeshOffsets(meshOffsets, *gpuMesh, payloads);
	RayTracingHitInstance& instance = payloads.Instances[input.GpuSceneSlot];

	instance = RayTracingHitInstance{
	    .FirstVertex = offsets.FirstVertex,
	    .FirstIndex = offsets.FirstIndex,
	    .VertexCount = offsets.VertexCount,
	    .IndexCount = offsets.IndexCount,
	    .MaterialSlot = draw.MaterialSlot,
	    .Flags = RayTracingHitData::InstanceFlag_Valid | (material.alphaMode == 0 ? RayTracingHitData::InstanceFlag_Opaque : 0u)
	        | (draw.Geometry.MeshKind == RenderMeshKind::Static ? RayTracingHitData::InstanceFlag_StaticMesh : 0u) | (material.bDoubleSided ? RayTracingHitData::InstanceFlag_TwoSided : 0u),
	    .GeometryFlags = BuildGeometryFlags(draw, material),
	    .RejectionReason = RayTracingHitData::Reason_None,
	    .AlphaMode = material.alphaMode,
	    .MaterialTextureFlags = material.textureFlags};

	instance.MorphTargetDeltaOffset = offsets.FirstMorphTargetDelta;
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
