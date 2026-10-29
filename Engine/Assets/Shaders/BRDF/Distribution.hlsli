#pragma once

#include "/Engine/Common/Constants.hlsli"
#include "/Engine/BRDF/Config.hlsli"

namespace BRDF
{
	namespace Distribution
	{
		float GGX(float3 normal, float3 halfVector, float alpha)
		{
			if (alpha == 0.0f)
			{
				return 0.0f;
			}

			const float NoH = saturate(dot(normal, halfVector));
			const float3 tangentHalfVector = cross(normal, halfVector);
			const float sinTheta2 = dot(tangentHalfVector, tangentHalfVector);
			const float a2 = alpha * alpha;
			const float denominator = sinTheta2 + a2 * NoH * NoH;
			const float ratio = alpha / denominator;
			return ratio * ratio * INV_PI;
		}

		float Beckmann(float NoH, float alpha)
		{
			const float a2 = alpha * alpha;
			const float NoH2 = NoH * NoH;
			const float exponent = (NoH2 - 1.0f) / (a2 * NoH2);
			return exp(exponent) / (PI * a2 * NoH2 * NoH2);
		}

		float BlinnPhong(float NoH, float alpha)
		{
			const float a2 = max(alpha * alpha, EPSILON);
			const float n = 2.0f / a2 - 2.0f;
			return (n + 2.0f) / (2.0f * PI) * pow(max(NoH, 0.0f), n);
		}

		float Evaluate(float3 normal, float3 halfVector, float alpha)
		{
			const float NoH = saturate(dot(normal, halfVector));
#if BRDF_DISTRIBUTION_MODEL == BRDF_DISTRIBUTION_GGX
			return GGX(normal, halfVector, alpha);
#elif BRDF_DISTRIBUTION_MODEL == BRDF_DISTRIBUTION_BECKMANN
			return Beckmann(NoH, alpha);
#elif BRDF_DISTRIBUTION_MODEL == BRDF_DISTRIBUTION_BLINN_PHONG
			return BlinnPhong(NoH, alpha);
#else
			return GGX(normal, halfVector, alpha);
#endif
		}
	}
}
