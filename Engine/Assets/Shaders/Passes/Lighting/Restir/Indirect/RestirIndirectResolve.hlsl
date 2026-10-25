Texture2D<float4> CurrentReservoirSampleTexture;
Texture2D<float4> CurrentReservoirWeightTexture;
Texture2D SkyTexture;
SamplerState SamplerLinearClamp;

#include "/Engine/Lighting/RestirIndirectReservoir.hlsli"

RWTexture2D<float4> IndirectDiffuse;
RWTexture2D<float4> IndirectSpecular;

#include "/Engine/Lighting/RayReconstructionGuides.hlsli"

void StoreIndirectLighting(uint2 pixelCoord, IndirectReservoir::Surface surface, PathLighting::Result path, float reservoirWeight)
{
	if (RestirIndirectEvaluateDiffuse != 0u)
	{
		const float3 diffuse = path.DiffuseContribution * reservoirWeight;
		IndirectDiffuse[pixelCoord] = float4(diffuse, any(diffuse > 0.0f) ? 1.0f : 0.0f);
	}

	bool hasSpecular = false;
	if (RestirIndirectEvaluateSpecular != 0u)
	{
		const float3 specular = path.SpecularContribution * reservoirWeight;
		hasSpecular = any(specular > 0.0f);
		IndirectSpecular[pixelCoord] = float4(specular, hasSpecular ? 1.0f : 0.0f);
	}

	if (WriteRayReconstructionGuides != 0u)
	{
		RayReconstructionGuides::WriteSpecularHitDistance(pixelCoord, path.FirstLighting.HitPositionWorld, surface.PathSurface.PositionWorld, hasSpecular && path.FirstLighting.Hit);
	}
}

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint width = 0u;
	uint height = 0u;
	IndirectDiffuse.GetDimensions(width, height);
	const uint2 pixelCoord = dispatchThreadId.xy;
	if (pixelCoord.x >= width || pixelCoord.y >= height)
	{
		return;
	}

	const IndirectReservoir::Surface surface = IndirectReservoir::LoadSurface(pixelCoord);

	const IndirectReservoir::Reservoir reservoir = IndirectReservoir::UnpackReservoir(CurrentReservoirSampleTexture.Load(int3(pixelCoord, 0)), CurrentReservoirWeightTexture.Load(int3(pixelCoord, 0)));

	if (!surface.Valid)
	{
		if (WriteRayReconstructionGuides != 0u)
		{
			RayReconstructionGuides::Clear(pixelCoord);
		}
		return;
	}

	if (WriteRayReconstructionGuides != 0u)
	{
		RayReconstructionGuides::WriteSurface(pixelCoord, surface.GBuffer, surface.PathSurface.ViewDirWorld);
	}

	if (!IndirectReservoir::IsValid(reservoir))
	{
		if (WriteRayReconstructionGuides != 0u)
		{
			RayReconstructionGuides::ClearSpecularHitDistance(pixelCoord);
		}
		return;
	}

	const PathLighting::Result path = IndirectReservoir::EvaluateCandidate(surface, reservoir.Selected, SkyTexture, SamplerLinearClamp);
	const float reservoirWeight = IndirectReservoir::GetFinalWeight(reservoir);

	StoreIndirectLighting(pixelCoord, surface, path, reservoirWeight);
}
