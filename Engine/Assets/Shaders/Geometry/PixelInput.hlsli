#pragma once

#include "/Engine/Geometry/Basis.hlsli"

namespace PS
{
	struct Input
	{
		float4 Position : SV_POSITION;
		float2 TexCoord : TEXCOORD0;
		float3 NormalWorld : TEXCOORD2;
		float4 TangentWorld : TEXCOORD3;
		float4 PrevClipPosition : TEXCOORD7;
		nointerpolation uint GpuSceneSlot : TEXCOORD8;
		bool IsFrontFace : SV_IsFrontFace;
	};

	void PrepareInput(inout Input input)
	{
		input.NormalWorld = normalize(input.NormalWorld);
		input.TangentWorld = float4(OrthonormalizeTangent(input.TangentWorld.xyz, input.NormalWorld), input.TangentWorld.w);
	}
}
