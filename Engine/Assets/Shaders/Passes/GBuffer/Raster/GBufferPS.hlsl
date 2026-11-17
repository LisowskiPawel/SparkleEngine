#include "/Engine/Resources/ViewUniformData.hlsli"

#include "/Engine/Geometry/PixelInput.hlsli"
#include "/Engine/Passes/GBuffer/Raster/RasterMaterial.hlsli"
#include "/Engine/Passes/GBuffer/GBufferPacking.hlsli"
#include "/Engine/Passes/GBuffer/MotionVector.hlsli"

struct GBufferOutput
{
	float4 BaseColor : SV_Target0;
	float4 WorldNormal : SV_Target1;
	float4 WorldTangent : SV_Target2;
	float4 Material : SV_Target3;
	float4 Emissive : SV_Target4;
	float4 Subsurface : SV_Target5;
	float2 MotionVector : SV_Target6;
	uint InstanceIdentity : SV_Target7;
};

void main(in PS::Input input, out GBufferOutput output)
{
	PS::PrepareInput(input);

	const RasterMaterial::Properties material = RasterMaterial::Sample(input);

	output.BaseColor = GBufferPacking::PackBaseColor(material.BaseColor, material.Alpha, material.AlphaMode);
	output.WorldNormal = GBufferPacking::PackWorldNormal(material.NormalWorld);
	const float3 worldTangent = OrthonormalizeTangent(input.TangentWorld.xyz, input.NormalWorld);
	output.WorldTangent = GBufferPacking::PackWorldTangent(input.bIsFrontFace ? worldTangent : -worldTangent);
	output.Material = GBufferPacking::PackMaterial(material.Metallic, material.Roughness, material.AmbientOcclusion, material.DielectricF0);
	output.Emissive = GBufferPacking::PackEmissive(material.Emissive);
	output.Subsurface = GBufferPacking::PackSubsurface(material.SubsurfaceColor, material.SubsurfaceStrength);

	output.InstanceIdentity = GBufferWriteInstanceIdentity != 0u ? input.GpuSceneSlot + 1u : 0u;
	output.MotionVector = MotionVectors::ComputeRaster(input.Position.xy, input.PrevClipPosition, ViewportSize);
}
