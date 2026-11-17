#pragma once

#include "RHI/Public/Shaders/ShaderTarget.h"
#include "RHI/Public/Shaders/ShaderMap.h"
#include "RHI/Public/Shaders/Authoring/ShaderParameterStruct.h"
#include "RHI/Public/Shaders/ShaderStage.h"

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

struct ShaderDescriptorBindingRemap final
{
	bool operator==(const ShaderDescriptorBindingRemap&) const;

	std::string Name;
	std::uint32_t Set = 0;
	std::uint32_t Binding = 0;
};

enum class ShaderCompileUnitKind : std::uint8_t
{
	EntryPoint = 0,
	Library,
};

enum class ShaderCompileFeatureFlags : std::uint8_t
{
	None = 0,
	InlineRayQuery = 1u << 0u,
	AccelerationStructure = 1u << 1u,
	DescriptorIndexing = 1u << 2u,
	RayTracingPipeline = 1u << 3u,
};

ShaderCompileFeatureFlags operator|(ShaderCompileFeatureFlags lhs, ShaderCompileFeatureFlags rhs) noexcept;
ShaderCompileFeatureFlags& operator|=(ShaderCompileFeatureFlags& lhs, ShaderCompileFeatureFlags rhs) noexcept;
bool HasShaderCompileFeature(ShaderCompileFeatureFlags value, ShaderCompileFeatureFlags flag) noexcept;

class ShaderSourceMountTable;

struct ShaderSourceFile final
{
	std::string VirtualPath;
	std::string Contents;
};

// Complete immutable input for one shader type and target compilation.
struct ShaderCompileRequest final
{
	explicit ShaderCompileRequest(const ShaderSourceMountTable& sourceMounts) noexcept;

	ShaderTypeId ShaderType = 0;
	std::string ShaderTypeName;
	std::string VirtualSourcePath;
	std::reference_wrapper<const ShaderSourceMountTable> SourceMounts;
	std::string SourceCode;
	std::vector<ShaderSourceFile> SourceFiles;
	std::string EntryPoint;
	ShaderStage Stage = ShaderStage::Count;
	ShaderTarget Target = kDefaultShaderTarget;
	ShaderCompileUnitKind UnitKind = ShaderCompileUnitKind::EntryPoint;
	ShaderCompileFeatureFlags RequiredFeatures = ShaderCompileFeatureFlags::None;
	std::optional<ShaderParameterStructDescriptor> ParameterStruct;

	bool bEnableDebugInfo = false;
	bool bEnableOptimizations = true;
	bool bTreatWarningsAsErrors = true;
	bool bStripDebugInfo = true;
	bool bCaptureDebugArtifacts = false;

	std::vector<std::string> Defines;
	std::vector<ShaderDescriptorBindingRemap> DescriptorBindingRemaps;
};
