#include "PCH.h"
#include "Scene/GpuScene/RenderGpuLightingPayloadBuilder.h"

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

void BuildRenderGpuLightingPayloads(const PreparedRenderScene& preparedScene, RenderGpuLightingPayloads& payloads)
{
	const std::size_t directionalLightCount = preparedScene.directionalLights.size();
	const std::size_t pointLightCount = preparedScene.pointLights.size();
	const std::size_t spotLightCount = preparedScene.spotLights.size();
	const std::size_t rectLightCount = preparedScene.rectLights.size();
	ValidateLightCount("directional", directionalLightCount, 2u);
	ValidateLightCount("point", pointLightCount, 1024u);
	ValidateLightCount("spot", spotLightCount, 1024u);
	ValidateLightCount("rect", rectLightCount, 1024u);

	payloads.DirectionalLights.clear();
	payloads.PointLights.clear();
	payloads.SpotLights.clear();
	payloads.RectLights.clear();

	payloads.Uniform = SceneLightingUniformData{
	    .DirectionalLightCount = static_cast<std::uint32_t>(directionalLightCount),
	    .PointLightCount = static_cast<std::uint32_t>(pointLightCount),
	    .SpotLightCount = static_cast<std::uint32_t>(spotLightCount),
	    .RectLightCount = static_cast<std::uint32_t>(rectLightCount)};

	payloads.DirectionalLights.reserve(directionalLightCount);
	payloads.PointLights.reserve(pointLightCount);
	payloads.SpotLights.reserve(spotLightCount);
	payloads.RectLights.reserve(rectLightCount);

	for (std::size_t lightIndex = 0; lightIndex < directionalLightCount; ++lightIndex)
	{
		const DirectionalLight& light = preparedScene.directionalLights[lightIndex];

		payloads.DirectionalLights.push_back(
		    DirectionalLightGpuData{
		        .Direction = {light.direction.x, light.direction.y, light.direction.z},
		        .Illuminance = light.illuminance,
		        .Color = {light.color.x, light.color.y, light.color.z},
		        .AngularSizeRadians = light.angularSizeRadians,
		        .CastShadow = light.castShadow ? 1u : 0u});
	}

	for (std::size_t lightIndex = 0; lightIndex < pointLightCount; ++lightIndex)
	{
		const PointLight& light = preparedScene.pointLights[lightIndex];

		payloads.PointLights.push_back(
		    PointLightGpuData{
		        .Position = {light.position.x, light.position.y, light.position.z},
		        .Range = light.range,
		        .Color = {light.color.x, light.color.y, light.color.z},
		        .LuminousIntensity = light.luminousIntensity,
		        .DistanceAttenuationCoefficients = light.distanceAttenuationCoefficients,
		        .Radius = light.radius,
		        .CastShadow = light.castShadow ? 1u : 0u});
	}

	for (std::size_t lightIndex = 0; lightIndex < spotLightCount; ++lightIndex)
	{
		const SpotLight& light = preparedScene.spotLights[lightIndex];

		payloads.SpotLights.push_back(
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
		        .CastShadow = light.castShadow ? 1u : 0u});
	}

	for (std::size_t lightIndex = 0; lightIndex < rectLightCount; ++lightIndex)
	{
		const RectLight& light = preparedScene.rectLights[lightIndex];

		payloads.RectLights.push_back(
		    RectLightGpuData{
		        .Position = {light.position.x, light.position.y, light.position.z},
		        .Width = light.width,
		        .Direction = {light.direction.x, light.direction.y, light.direction.z},
		        .Height = light.height,
		        .Tangent = {light.tangent.x, light.tangent.y, light.tangent.z},
		        .Luminance = light.luminance,
		        .Color = {light.color.x, light.color.y, light.color.z},
		        .CastShadow = light.castShadow ? 1u : 0u});
	}
}
