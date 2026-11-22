#include "PCH.h"

#include "Cli/ListBackendsCommand.h"

#include "Backend/ShaderBackends.h"
#include "Cli/ShaderCompilerExitCodes.h"
#include "Core/Public/Strings/StringUtils.h"

#include <exception>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>

static std::string BuildTargetList(std::span<const ShaderTarget> targets)
{
	std::vector<std::string_view> names;
	for (const ShaderTarget target : targets)
	{
		names.emplace_back(GetShaderTargetName(target));
	}
	return names.empty() ? "none" : Strings::Join(names, ", ");
}

static std::string BuildRayTracingFeatureList(const ShaderBackendCapabilities& capabilities)
{
	std::vector<std::string_view> features;
	if (capabilities.bSupportsDxilRayTracingLibrary)
	{
		features.emplace_back("dxil-rt-library");
	}
	if (capabilities.bSupportsSpirVRayTracingLibrary)
	{
		features.emplace_back("spirv-rt-library");
	}
	if (capabilities.bSupportsDxilInlineRayQuery)
	{
		features.emplace_back("dxil-inline-ray-query");
	}
	if (capabilities.bSupportsSpirVInlineRayQuery)
	{
		features.emplace_back("spirv-inline-ray-query");
	}
	return features.empty() ? "none" : Strings::Join(features, ", ");
}

int ListBackends()
{
	try
	{
		const std::vector<ShaderBackendDescriptor> backends = ListShaderBackends();
		const std::string targets = BuildTargetList(ListShaderCodegenTargets());
		const std::string formats = Strings::Join(ListShaderBinaryFormats(), ", ");
		std::cout << "ShaderCompiler: " << backends.size() << " backend(s) registered\n";
		for (const ShaderBackendDescriptor& backend : backends)
		{
			const std::string features = BuildRayTracingFeatureList(backend.Capabilities);
			std::cout << "  name='" << backend.Name << "'"
			          << " available=true sourceExtensions='" << backend.SourceExtension << "'"
			          << " codegenTargets='" << targets << "'"
			          << " binaryFormats='" << formats << "'"
			          << " rt='" << features << "'"
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
	catch (const std::exception& error)
	{
		std::cerr << "ShaderCompiler: list-backends failed - " << error.what() << "\n";
		return kExitCodeCookFailure;
	}
}
