#include "PCH.h"
#include "Scene/GpuScene/Lighting/RenderGpuLightingPayloadBuilder.h"

#include "Core/Public/Diagnostics/Verify.h"
#include "Scene/Preparation/PreparedRenderScene.h"
#include "ShaderData/SceneLightingUniformData.h"

#include <format>
#include <string_view>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_renderGpuLightingPayloadBuilderLogger, "Renderer.RenderGpuLightingPayloadBuilder");

static void ValidateLightCount(std::string_view lightKind, std::size_t count, std::size_t limit)
{
	if (count > limit)
	{
		Diagnostics::Fatal(g_renderGpuLightingPayloadBuilderLogger, __FILE__, __LINE__, std::format("Scene contains {} {} lights, exceeding the renderer limit of {}.", count, lightKind, limit));
	}
}

static void PackDirectionalLights(const RenderLightCollection<DirectionalLight>& lights, std::vector<DirectionalLightGpuData>& output)
{
	output.clear();
	output.reserve(lights.size());
	for (std::size_t lightIndex = 0; lightIndex < lights.size(); ++lightIndex)
	{
		const DirectionalLight& light = lights[lightIndex];

		output.push_back(
		    DirectionalLightGpuData{
		        .Direction = {light.direction.x, light.direction.y, light.direction.z},
		        .Illuminance = light.illuminance,
		        .Color = {light.color.x, light.color.y, light.color.z},
		        .AngularSizeRadians = light.angularSizeRadians,
		        .CastShadow = light.bCastShadow ? 1u : 0u});
	}
}

static void PackPointLights(const RenderLightCollection<PointLight>& lights, std::vector<PointLightGpuData>& output)
{
	output.clear();
	output.reserve(lights.size());
	for (std::size_t lightIndex = 0; lightIndex < lights.size(); ++lightIndex)
	{
		const PointLight& light = lights[lightIndex];

		output.push_back(
		    PointLightGpuData{
		        .Position = {light.position.x, light.position.y, light.position.z},
		        .Range = light.range,
		        .Color = {light.color.x, light.color.y, light.color.z},
		        .LuminousIntensity = light.luminousIntensity,
		        .DistanceAttenuationCoefficients = light.distanceAttenuationCoefficients,
		        .Radius = light.radius,
		        .CastShadow = light.bCastShadow ? 1u : 0u});
	}
}

static void PackSpotLights(const RenderLightCollection<SpotLight>& lights, std::vector<SpotLightGpuData>& output)
{
	output.clear();
	output.reserve(lights.size());
	for (std::size_t lightIndex = 0; lightIndex < lights.size(); ++lightIndex)
	{
		const SpotLight& light = lights[lightIndex];

		output.push_back(
		    SpotLightGpuData{
		        .Position = {light.position.x, light.position.y, light.position.z},
		        .Range = light.range,
		        .Direction = {light.direction.x, light.direction.y, light.direction.z},
		        .InnerAngleCosine = light.innerAngleCosine,
		        .Color = {light.color.x, light.color.y, light.color.z},
		        .LuminousIntensity = light.luminousIntensity,
		        .DistanceAttenuationCoefficients = light.distanceAttenuationCoefficients,
		        .Radius = light.radius,
		        .OuterAngleCosine = light.outerAngleCosine,
		        .CastShadow = light.bCastShadow ? 1u : 0u});
	}
}

static void PackRectLights(const RenderLightCollection<RectLight>& lights, std::vector<RectLightGpuData>& output)
{
	output.clear();
	output.reserve(lights.size());
	for (std::size_t lightIndex = 0; lightIndex < lights.size(); ++lightIndex)
	{
		const RectLight& light = lights[lightIndex];

		output.push_back(
		    RectLightGpuData{
		        .Position = {light.position.x, light.position.y, light.position.z},
		        .Width = light.width,
		        .Direction = {light.direction.x, light.direction.y, light.direction.z},
		        .Height = light.height,
		        .Tangent = {light.tangent.x, light.tangent.y, light.tangent.z},
		        .Luminance = light.luminance,
		        .Color = {light.color.x, light.color.y, light.color.z},
		        .CastShadow = light.bCastShadow ? 1u : 0u});
	}
}

static void ValidateLightCounts(const PreparedRenderScene& preparedScene)
{
	const std::size_t directionalLightCount = preparedScene.directionalLights.size();
	const std::size_t pointLightCount = preparedScene.pointLights.size();
	const std::size_t spotLightCount = preparedScene.spotLights.size();
	const std::size_t rectLightCount = preparedScene.rectLights.size();

	ValidateLightCount("directional", directionalLightCount, 2u);
	ValidateLightCount("point", pointLightCount, 1024u);
	ValidateLightCount("spot", spotLightCount, 1024u);
	ValidateLightCount("rect", rectLightCount, 1024u);
}

static SceneLightingUniformData BuildSceneLightingUniform(const PreparedRenderScene& preparedScene) noexcept
{
	return SceneLightingUniformData{
	    .DirectionalLightCount = static_cast<std::uint32_t>(preparedScene.directionalLights.size()),
	    .PointLightCount = static_cast<std::uint32_t>(preparedScene.pointLights.size()),
	    .SpotLightCount = static_cast<std::uint32_t>(preparedScene.spotLights.size()),
	    .RectLightCount = static_cast<std::uint32_t>(preparedScene.rectLights.size())};
}

void BuildRenderGpuLightingPayloads(const PreparedRenderScene& preparedScene, RenderGpuLightingPayloads& payloads)
{
	ValidateLightCounts(preparedScene);

	PackDirectionalLights(preparedScene.directionalLights, payloads.DirectionalLights);
	PackPointLights(preparedScene.pointLights, payloads.PointLights);
	PackSpotLights(preparedScene.spotLights, payloads.SpotLights);
	PackRectLights(preparedScene.rectLights, payloads.RectLights);

	payloads.Uniform = BuildSceneLightingUniform(preparedScene);
}
