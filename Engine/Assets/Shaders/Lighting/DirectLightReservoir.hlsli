#ifndef SPARKLE_DIRECT_LIGHT_RESERVOIR_HLSLI
#define SPARKLE_DIRECT_LIGHT_RESERVOIR_HLSLI

#include "/Engine/Resources/ViewCameraUniformData.hlsli"

#include "/Engine/Common/Color.hlsli"
#include "/Engine/Common/Random.hlsli"
#include "/Engine/Lighting/DirectLightReservoirData.hlsli"
#include "/Engine/Lighting/RestirReservoirCommon.hlsli"
#include "/Engine/Lighting/SurfaceLighting.hlsli"
#include "/Engine/Passes/GBuffer/GBufferUtils.hlsli"

namespace DirectReservoir
{

	struct Surface
	{
		bool Valid;
		GBufferData GBuffer;
		float3 PositionWorld;
		float3 ViewDirWorld;
		float ViewDistance;
		bool EvaluateSubsurface;
	};

	Surface LoadSurface(uint2 pixelCoord)
	{
		Surface surface;
		surface.Valid = false;
		surface.GBuffer = LoadGBuffer(pixelCoord);
		surface.PositionWorld = 0.0f.xxx;
		surface.ViewDirWorld = 0.0f.xxx;
		surface.ViewDistance = 0.0f;
		surface.EvaluateSubsurface = false;

		if (IsSkyPixel(surface.GBuffer.SceneDepth))
		{
			return surface;
		}

		surface.PositionWorld = ReconstructGBufferWorldPosition(pixelCoord, surface.GBuffer.SceneDepth, InvViewMTX, InvProjectionMTX);
		const float3 cameraToSurface = surface.PositionWorld - Position;
		surface.ViewDistance = length(cameraToSurface);
		surface.ViewDirWorld = surface.ViewDistance > 1.0e-5f ? -cameraToSurface / surface.ViewDistance : 0.0f.xxx;
		surface.EvaluateSubsurface = DirectLightingEvaluateSubsurface != 0u && HasSubsurface(surface.GBuffer);
		surface.Valid = true;
		return surface;
	}

	float4 PackSurface(Surface surface)
	{
		return RestirReservoirCommon::PackSurface(surface.Valid, surface.GBuffer.NormalWorld, surface.ViewDistance);
	}

	bool AreSurfacesCompatible(Surface surface, float4 packedSurface)
	{
		return RestirReservoirCommon::AreSurfacesCompatible(surface.Valid, surface.GBuffer.NormalWorld, surface.ViewDistance, packedSurface);
	}

	SurfaceLighting::Surface BuildLightingSurface(GBufferData gBuffer, float3 viewDirWorld, bool evaluateSubsurface)
	{
		SurfaceLighting::Surface lightingSurface = {viewDirWorld,
		                                            gBuffer.NormalWorld,
		                                            gBuffer.BaseColor,
		                                            gBuffer.Roughness,
		                                            gBuffer.Metallic,
		                                            gBuffer.DielectricF0,
		                                            gBuffer.SubsurfaceColor,
		                                            gBuffer.SubsurfaceStrength,
		                                            DirectLightingEvaluateDiffuse != 0u,
		                                            DirectLightingEvaluateSpecular != 0u,
		                                            evaluateSubsurface};

		return lightingSurface;
	}

	float3 EvaluateRawContribution(Surface surface, Reservoir reservoir)
	{
		if (!surface.Valid || !IsValid(reservoir))
		{
			return 0.0f.xxx;
		}

		const LightSampling::DirectLightSample lightSample = ReplayLightSample(reservoir, surface.PositionWorld);

		const SurfaceLighting::Surface lightingSurface = BuildLightingSurface(surface.GBuffer, surface.ViewDirWorld, surface.EvaluateSubsurface);

		const SurfaceLighting::Contribution contribution = SurfaceLighting::EvaluateDirectLightSample(lightingSurface, lightSample, 1.0f);

		return max(contribution.Diffuse + contribution.Specular + contribution.Subsurface, 0.0f.xxx);
	}

	SurfaceLighting::Contribution EvaluateSelectedLighting(uint2 pixelCoord, GBufferData gBuffer, Reservoir reservoir, float visibility)
	{
		const float3 positionWorld = ReconstructGBufferWorldPosition(pixelCoord, gBuffer.SceneDepth, InvViewMTX, InvProjectionMTX);
		const float3 viewDirWorld = normalize(Position - positionWorld);

		SurfaceLighting::Contribution directLighting = (SurfaceLighting::Contribution)0;

		const bool evaluateSubsurface = DirectLightingEvaluateSubsurface != 0u && HasSubsurface(gBuffer);
		if (!IsValid(reservoir))
		{
			return directLighting;
		}

		const LightSampling::DirectLightSample lightSample = ReplayLightSample(reservoir, positionWorld);
		const float reservoirWeight = GetFinalWeight(reservoir);
		if (!lightSample.Valid || reservoirWeight <= 0.0f)
		{
			return directLighting;
		}

		const SurfaceLighting::Surface lightingSurface = BuildLightingSurface(gBuffer, viewDirWorld, evaluateSubsurface);

		const SurfaceLighting::Contribution contribution = SurfaceLighting::EvaluateDirectLightSample(lightingSurface, lightSample, visibility);

		directLighting.Diffuse += contribution.Diffuse * reservoirWeight;
		directLighting.Specular += contribution.Specular * reservoirWeight;
		directLighting.Subsurface += contribution.Subsurface * reservoirWeight;
		return directLighting;
	}

	float EvaluateTargetPdf(Surface surface, Reservoir reservoir)
	{
		return CommonColor::LuminanceRec709(EvaluateRawContribution(surface, reservoir));
	}

	float ComputeSourcePdf(DirectLightSampling::LightCandidate candidate, float3 positionWorld, float2 shapeSample)
	{
		if (!DirectLightSampling::IsValid(candidate))
		{
			return 0.0f;
		}

		const LightSampling::DirectLightSample sample = DirectLightSampling::SampleDirectLight(candidate, positionWorld, shapeSample);
		return max(candidate.SelectionPdf * sample.PdfW, 0.0f);
	}

	bool StreamWeightedSample(inout Reservoir reservoir, DirectLightSampling::LightCandidate candidate, float2 shapeSample, float targetPdf, float sourcePdf, float random)
	{
		reservoir.M += 1.0f;
		if (!DirectLightSampling::IsValid(candidate) || targetPdf <= MinPdf || sourcePdf <= MinPdf)
		{
			return false;
		}

		const float sampleWeight = targetPdf / sourcePdf;
		const float newWeightSum = reservoir.WeightSum + sampleWeight;
		const bool selected = random * newWeightSum <= sampleWeight;
		reservoir.WeightSum = newWeightSum;
		if (selected)
		{
			reservoir.Candidate = candidate;
			reservoir.Candidate.SelectionPdf = 1.0f;
			reservoir.ShapeSample = shapeSample;
			reservoir.TargetPdf = targetPdf;
			reservoir.Valid = 1.0f;
		}
		return selected;
	}

	bool CombineReservoir(inout Reservoir reservoir, Reservoir candidateReservoir, Surface surface, float maxM, float random)
	{
		if (!surface.Valid || candidateReservoir.M <= 0.0f)
		{
			return false;
		}

		const float candidateM = candidateReservoir.M;
		const float acceptedM = min(candidateM, maxM);
		reservoir.M += acceptedM;
		if (!IsValid(candidateReservoir))
		{
			return false;
		}

		const float targetPdf = EvaluateTargetPdf(surface, candidateReservoir);
		if (targetPdf <= MinPdf)
		{
			return false;
		}

		const float historyScale = acceptedM / candidateM;
		const float sampleWeight = candidateReservoir.WeightSum * historyScale * targetPdf / max(candidateReservoir.TargetPdf, MinPdf);
		if (sampleWeight <= MinPdf)
		{
			return false;
		}

		const float newWeightSum = reservoir.WeightSum + sampleWeight;
		const bool selected = random * newWeightSum <= sampleWeight;
		reservoir.WeightSum = newWeightSum;
		if (selected)
		{
			reservoir.Candidate = candidateReservoir.Candidate;
			reservoir.ShapeSample = candidateReservoir.ShapeSample;
			reservoir.TargetPdf = targetPdf;
			reservoir.Valid = 1.0f;
		}
		return selected;
	}

	Reservoir BuildInitialReservoir(Surface surface, uint2 pixelCoord)
	{
		if (!surface.Valid)
		{
			return EmptyReservoir();
		}

		Reservoir reservoir = EmptyReservoir();
		uint rng = RestirReservoirCommon::BuildSeed(pixelCoord, 0xC0FFEEu);
		[unroll]
		for (uint candidateIndex = 0u; candidateIndex < RestirReservoirCommon::InitialCandidateCount; ++candidateIndex)
		{
			const DirectLightSampling::LightCandidate candidate = DirectLightSampling::SampleUniformLightCandidate(CommonRandom::Random01(rng));
			const float2 shapeSample = CommonRandom::Random02(rng);
			Reservoir candidateReservoir = EmptyReservoir();
			candidateReservoir.Candidate = candidate;
			candidateReservoir.ShapeSample = shapeSample;
			candidateReservoir.WeightSum = 1.0f;
			candidateReservoir.TargetPdf = 1.0f;
			candidateReservoir.M = 1.0f;
			candidateReservoir.Valid = DirectLightSampling::IsValid(candidate) ? 1.0f : 0.0f;

			const float targetPdf = EvaluateTargetPdf(surface, candidateReservoir);
			const float sourcePdf = ComputeSourcePdf(candidate, surface.PositionWorld, shapeSample);
			StreamWeightedSample(reservoir, candidate, shapeSample, targetPdf, sourcePdf, CommonRandom::Random01(rng));
		}

		return reservoir;
	}
}

#endif
