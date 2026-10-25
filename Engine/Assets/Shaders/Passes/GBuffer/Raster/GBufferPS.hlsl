#include "/Engine/Resources/ViewUniformData.hlsli"

#include "/Engine/Geometry/PixelInput.hlsli"
#include "/Engine/Material/Material.hlsli"
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

void main(in PS::Input Input, out GBufferOutput Output)
{
	PS::PrepareInput(Input);

	Material::Properties MatProps = Material::Sample(Input);

	Output.BaseColor = GBufferPacking::PackBaseColor(MatProps.BaseColor, MatProps.Alpha, MatProps.AlphaMode, Material::AlphaModeBlend);
	Output.WorldNormal = GBufferPacking::PackWorldNormal(MatProps.NormalWorld);
	const float3 worldTangent = OrthonormalizeTangent(Input.TangentWorld.xyz, Input.NormalWorld);
	Output.WorldTangent = GBufferPacking::PackWorldTangent(Input.IsFrontFace ? worldTangent : -worldTangent);
	Output.Material = GBufferPacking::PackMaterial(MatProps.Metallic, MatProps.Roughness, MatProps.AmbientOcclusion, MatProps.DielectricF0);
	Output.Emissive = GBufferPacking::PackEmissive(MatProps.Emissive);
	Output.Subsurface = GBufferPacking::PackSubsurface(MatProps.SubsurfaceColor, MatProps.SubsurfaceStrength);

	Output.InstanceIdentity = GBufferWriteInstanceIdentity != 0u ? Input.GpuSceneSlot + 1u : 0u;
	Output.MotionVector = MotionVectors::ComputeRaster(Input.Position.xy, Input.PrevClipPosition, ViewportSize);
}
