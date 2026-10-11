#pragma once

#include "/Engine/Common/Constants.hlsli"
#include "/Engine/Common/Math.hlsli"

namespace CommonSampling
{
	struct OrthonormalBasis
	{
		float3 Tangent;
		float3 Bitangent;
	};

	OrthonormalBasis BuildOrthonormalBasis(float3 normal)
	{
		const float3 axis = SafeNormalize(normal);
		const float signValue = axis.z >= 0.0f ? 1.0f : -1.0f;
		const float basisA = -1.0f / (signValue + axis.z);
		const float basisB = axis.x * axis.y * basisA;

		OrthonormalBasis basis;
		basis.Tangent = float3(1.0f + signValue * axis.x * axis.x * basisA, signValue * basisB, -signValue * axis.x);
		basis.Bitangent = float3(basisB, signValue + axis.y * axis.y * basisA, -axis.y);
		return basis;
	}

	float3 SampleConeDirection(float3 axis, float coneHalfAngleRadians, float2 sample)
	{
		const float3 normalizedAxis = SafeNormalize(axis);
		const float clampedHalfAngle = max(coneHalfAngleRadians, 0.0f);
		if (clampedHalfAngle <= 0.0f)
		{
			return normalizedAxis;
		}

		const float cosThetaMin = cos(clampedHalfAngle);
		const float cosTheta = lerp(1.0f, cosThetaMin, sample.x);
		const float sinTheta = sqrt(saturate(1.0f - cosTheta * cosTheta));
		const float phi = TWO_PI * sample.y;

		const OrthonormalBasis basis = BuildOrthonormalBasis(normalizedAxis);
		return SafeNormalize(basis.Tangent * (cos(phi) * sinTheta) + basis.Bitangent * (sin(phi) * sinTheta) + normalizedAxis * cosTheta);
	}

	float3 SampleSpherePoint(float3 center, float radius, float3 referenceDirection, float2 sample)
	{
		if (radius <= 0.0f)
		{
			return center;
		}

		const float z = 1.0f - 2.0f * sample.x;
		const float radial = sqrt(saturate(1.0f - z * z));
		const float phi = TWO_PI * sample.y;

		const float3 axis = length(referenceDirection) > EPSILON ? SafeNormalize(referenceDirection) : float3(0.0f, 1.0f, 0.0f);
		const OrthonormalBasis basis = BuildOrthonormalBasis(axis);
		const float3 offset = basis.Tangent * (radial * cos(phi)) + basis.Bitangent * (radial * sin(phi)) + axis * z;
		return center + offset * radius;
	}

	float3 SampleDiskPoint(float3 center, float3 normal, float radius, float2 sample)
	{
		if (radius <= 0.0f)
		{
			return center;
		}

		const float sampleRadius = radius * sqrt(sample.x);
		const float phi = TWO_PI * sample.y;

		const OrthonormalBasis basis = BuildOrthonormalBasis(normal);
		return center + basis.Tangent * (sampleRadius * cos(phi)) + basis.Bitangent * (sampleRadius * sin(phi));
	}

	float3 SampleCosineHemisphereDirection(float3 normal, float2 sample)
	{
		const float phi = TWO_PI * sample.x;
		const float cosTheta = sqrt(saturate(1.0f - sample.y));
		const float sinTheta = sqrt(saturate(sample.y));

		const OrthonormalBasis basis = BuildOrthonormalBasis(normal);

		return SafeNormalize(basis.Tangent * (cos(phi) * sinTheta) + basis.Bitangent * (sin(phi) * sinTheta) + SafeNormalize(normal) * cosTheta, SafeNormalize(normal));
	}
}
