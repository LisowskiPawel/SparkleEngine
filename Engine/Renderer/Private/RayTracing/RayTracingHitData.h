#pragma once

#include "ShaderData/MaterialTextureMappingData.h"

#include <DirectXMath.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace RayTracingHitData
{
	inline constexpr std::uint32_t InstanceFlagValid = 1u << 0u;
	inline constexpr std::uint32_t InstanceFlagTwoSided = 1u << 2u;
	inline constexpr std::uint32_t MaterialFlagEmissive = 1u << 5u;
}

struct RayTracingHitVertex
{
	DirectX::XMFLOAT3 Position = {};
	DirectX::XMFLOAT3 Normal = {};
	DirectX::XMFLOAT4 Tangent = {1.0f, 0.0f, 0.0f, 1.0f};
	DirectX::XMFLOAT2 TexCoord0 = {};
	DirectX::XMFLOAT4 Color = {1.0f, 1.0f, 1.0f, 1.0f};
};

static_assert(std::is_standard_layout_v<RayTracingHitVertex>, "RayTracingHitVertex must be standard-layout");
static_assert(std::is_trivially_copyable_v<RayTracingHitVertex>, "RayTracingHitVertex must be trivially copyable");
static_assert(sizeof(RayTracingHitVertex) == 64, "RayTracingHitVertex must match the shader layout");

struct RayTracingHitInstance
{
	std::uint32_t FirstVertex = 0u;
	std::uint32_t FirstIndex = 0u;
	std::uint32_t IndexCount = 0u;
	std::uint32_t MaterialSlot = 0u;
	std::uint32_t Flags = 0u;
	std::uint32_t MorphTargetDeltaOffset = 0u;
};

static_assert(std::is_standard_layout_v<RayTracingHitInstance>, "RayTracingHitInstance must be standard-layout");
static_assert(std::is_trivially_copyable_v<RayTracingHitInstance>, "RayTracingHitInstance must be trivially copyable");
static_assert(sizeof(RayTracingHitInstance) == 24, "RayTracingHitInstance must match the shader layout");
static_assert(offsetof(RayTracingHitInstance, FirstVertex) == 0);
static_assert(offsetof(RayTracingHitInstance, FirstIndex) == 4);
static_assert(offsetof(RayTracingHitInstance, IndexCount) == 8);
static_assert(offsetof(RayTracingHitInstance, MaterialSlot) == 12);
static_assert(offsetof(RayTracingHitInstance, Flags) == 16);
static_assert(offsetof(RayTracingHitInstance, MorphTargetDeltaOffset) == 20);

struct RayTracingHitMaterial
{
	DirectX::XMFLOAT4 BaseColor = {1.0f, 1.0f, 1.0f, 1.0f};
	DirectX::XMFLOAT3 EmissiveColor = {};
	float Metallic = 0.0f;
	float Roughness = 0.5f;
	float F0 = 0.04f;
	float AlphaCutoff = 0.5f;
	std::uint32_t AlphaMode = 0u;
	std::uint32_t TextureFlags = 0u;
	DirectX::XMFLOAT3 SubsurfaceColor = {};
	float SubsurfaceStrength = 0.0f;
	std::uint32_t Flags = 0u;
	DirectX::XMUINT4 TextureIndices0 = {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX};
	DirectX::XMUINT4 TextureIndices1 = {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX};
	std::array<MaterialTextureMappingData, 8> TextureMappings = {};
};

static_assert(std::is_standard_layout_v<RayTracingHitMaterial>, "RayTracingHitMaterial must be standard-layout");
static_assert(std::is_trivially_copyable_v<RayTracingHitMaterial>, "RayTracingHitMaterial must be trivially copyable");
static_assert(sizeof(RayTracingHitMaterial) == 360, "RayTracingHitMaterial must match the shader layout");
