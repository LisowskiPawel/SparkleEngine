#include "PCH.h"

#include "Cli/ListBackendsCommand.h"

#include "Backend/ShaderBackendFactory.h"
#include "Constants/ShaderCompilerConstants.h"

#include <iostream>

static std::string BuildStringList(std::span<const std::string_view> values)
{
	std::string result;
	for (const std::string_view value : values)
	{
		if (!result.empty())
		{
			result += ", ";
		}
		result += value;
	}
	if (result.empty())
	{
		result = "none";
	}
	return result;
}

static std::string BuildTargetList(std::span<const ShaderTarget> targets)
{
	std::string result;
	for (const ShaderTarget target : targets)
	{
		if (!result.empty())
		{
			result += ", ";
		}
		result += GetShaderTargetName(target);
	}
	if (result.empty())
	{
		result = "none";
	}
	return result;
}

static std::string BuildRayTracingFeatureList(const ShaderBackendCapabilities& capabilities)
{
	std::string features;
	if (capabilities.bSupportsDxilRayTracingLibrary)
	{
		features = "dxil-rt-library";
	}
	if (capabilities.bSupportsSpirVRayTracingLibrary)
	{
		if (!features.empty())
		{
			features += ", ";
		}
		features += "spirv-rt-library";
	}
	if (capabilities.bSupportsDxilInlineRayQuery)
	{
		if (!features.empty())
		{
			features += ", ";
		}
		features += "dxil-inline-ray-query";
	}
	if (capabilities.bSupportsSpirVInlineRayQuery)
	{
		if (!features.empty())
		{
			features += ", ";
		}
		features += "spirv-inline-ray-query";
	}
	if (features.empty())
	{
		features = "none";
	}
	return features;
}

int ListBackends()
{
	const std::vector<ShaderBackendDescriptor> backends = ListShaderBackends();
	std::cout << "ShaderCompiler: " << backends.size() << " backend(s) registered\n";
	for (const ShaderBackendDescriptor& backend : backends)
	{
		std::cout << "  name='" << backend.Name << "'"
		          << " available=true sourceExtensions='" << backend.SourceExtension << "'"
		          << " codegenTargets='" << BuildTargetList(ListShaderCodegenTargets()) << "'"
		          << " binaryFormats='" << BuildStringList(ListShaderBinaryFormats()) << "'"
		          << " rt='" << BuildRayTracingFeatureList(backend.Capabilities) << "'"
		          << " version=" << backend.Version;
		std::cout << "\n";
	}

	const std::span<const std::string_view> binaryFormats = ListShaderBinaryFormats();
	std::cout << "ShaderCompiler: " << binaryFormats.size() << " binary format(s) registered\n";
	for (const std::string_view binaryFormat : binaryFormats)
	{
		std::cout << "  name='" << binaryFormat << "'"
		          << " available=true\n";
	}

	const std::span<const ShaderTarget> codegenTargets = ListShaderCodegenTargets();
	std::cout << "ShaderCompiler: " << codegenTargets.size() << " codegen target(s) registered\n";
	for (const ShaderTarget codegenTarget : codegenTargets)
	{
		std::cout << "  name='" << GetShaderTargetName(codegenTarget) << "'"
		          << " binaryFormat='" << (IsDxilTarget(codegenTarget) ? "Dxil" : "SpirV") << "'"
		          << " available=true\n";
	}

	return kExitCodeSuccess;
}
