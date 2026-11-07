#pragma once

namespace PathSample
{
	static const uint LobeDiffuse = 1u;
	static const uint LobeSpecular = 2u;

	struct DirectionSample
	{
		float3 DirectionWorld;
		float PdfW;
		float3 DiffuseThroughput;
		float3 SpecularThroughput;
		bool bDelta;
		bool bHasSupport;
	};

	struct LightingResult
	{
		bool bHit;
		float3 IncidentRadiance;
		float3 HitPositionWorld;
	};
}
