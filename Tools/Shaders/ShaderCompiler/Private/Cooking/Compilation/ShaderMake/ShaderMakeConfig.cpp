#include "PCH.h"

#include "Cooking/Compilation/ShaderMake/ShaderMakeConfig.h"
#include "Cooking/Compilation/ShaderMake/ShaderMakeProducer.h"
#include "Cooking/Compilation/ShaderCompileJob.h"
#include "Compiler/ShaderCompileProfile.h"
#include "Core/Public/Diagnostics/Error.h"

#include <format>

static const char* GetVulkanVersion(ShaderTarget target)
{
	switch (target)
	{
		case ShaderTarget::SpirV14:
			return "1.1spirv1.4";
		case ShaderTarget::SpirV15:
			return "1.2";
		default:
			return "1.3";
	}
}

static std::string BuildDxcOptions(const ShaderMakeProducer& producer, ShaderMakeOperation operation)
{
	const ShaderCompileRequest& request = producer.Job.Request;
	std::string options = "-HV 2021 -Ges -all_resources_bound";
	if (request.bTreatWarningsAsErrors)
	{
		options += " -WX";
	}
	if (request.UnitKind == ShaderCompileUnitKind::Library)
	{
		options += " -exports " + request.EntryPoint;
		if (IsDxilTarget(request.Target))
		{
			options += " -auto-binding-space 0";
		}
	}
	if (IsDxilTarget(request.Target))
	{
		options += " -Qstrip_reflect";
		if (request.bStripDebugInfo)
		{
			options += " -Qstrip_debug";
		}
		if (operation == ShaderMakeOperation::Compile)
		{
			options += " -Fre " + producer.Reflection.generic_string();
		}
	}
	else
	{
		if constexpr (bSpirVUseDirectXBufferLayout)
		{
			options += " -fvk-use-dx-layout";
		}
		if constexpr (bSpirVUseUnknownStorageImageFormat)
		{
			options += " -fspv-use-unknown-image-format";
		}
		if (HasShaderCompileFeature(request.RequiredFeatures, ShaderCompileFeatureFlags::InlineRayQuery))
		{
			options += " -fspv-extension=SPV_KHR_ray_query";
		}
		const bool bRayPipeline = HasShaderCompileFeature(request.RequiredFeatures, ShaderCompileFeatureFlags::RayTracingPipeline);
		const bool bAccelerationStructure = HasShaderCompileFeature(request.RequiredFeatures, ShaderCompileFeatureFlags::AccelerationStructure);
		if (bRayPipeline || bAccelerationStructure)
		{
			options += " -fspv-extension=SPV_KHR_ray_tracing";
		}
		if (HasShaderCompileFeature(request.RequiredFeatures, ShaderCompileFeatureFlags::DescriptorIndexing))
		{
			options += " -fspv-extension=SPV_EXT_descriptor_indexing";
		}
	}
	if (request.bEnableDebugInfo)
	{
		options += IsSpirVTarget(request.Target) ? " -Zi -fspv-debug=vulkan-with-source" : " -Zi -Qsource_in_debug_module";
		if (operation == ShaderMakeOperation::Compile && IsDxilTarget(request.Target))
		{
			options += " -Fd " + producer.Symbols.generic_string() + "/";
		}
	}
	if (operation == ShaderMakeOperation::Preprocess)
	{
		options += " -P -Fi " + producer.Preprocessed.generic_string();
	}
	return options;
}

static std::string BuildSlangOptions(const ShaderMakeProducer& producer)
{
	const ShaderCompileRequest& request = producer.Job.Request;
	std::string options = "-module-name SparkleShader -stage ";
	options += GetSlangShaderStageName(request.Stage);
	if (request.UnitKind == ShaderCompileUnitKind::Library)
	{
		options += " -entry " + request.EntryPoint;
	}
	options += request.bEnableDebugInfo ? " -g2" : " -g0";
	if (request.bTreatWarningsAsErrors)
	{
		options += " -warnings-as-errors all";
	}
	if (IsSpirVTarget(request.Target))
	{
		options += " -emit-spirv-directly -fvk-use-entrypoint-name -capability sm_6_6";
		if constexpr (bSpirVUseDirectXBufferLayout)
		{
			options += " -fvk-use-dx-layout";
		}
		if constexpr (bSpirVUseUnknownStorageImageFormat)
		{
			options += " -default-image-format-unknown";
		}
		options += " -fvk-b-shift 0 all -fvk-t-shift 0 all -fvk-s-shift 0 all -fvk-u-shift 0 all";
	}
	else
	{
		if (request.bEnableDebugInfo)
		{
			options += " -gpdb";
		}
		if (request.bStripDebugInfo)
		{
			options += " -Xdxc -Qstrip_debug";
		}
		// This adds a JSON target: preceding target-scoped options belong to DXIL.
		options += " -reflection-json " + producer.Reflection.generic_string();
	}
	return options;
}

std::string BuildShaderMakeConfiguration(std::span<const ShaderMakeProducer> producers, ShaderMakeOperation operation)
{
	std::string configuration;
	for (const ShaderMakeProducer& producer : producers)
	{
		const ShaderCompileJob& job = producer.Job;
		const ShaderCompileRequest& request = job.Request;
		const std::string profile = request.UnitKind == ShaderCompileUnitKind::Library ? "lib" : GetShaderStagePrefix(request.Stage);
		const std::string options = job.BackendName == "dxc" ? BuildDxcOptions(producer, operation) : BuildSlangOptions(producer);
		const std::string shaderModel = std::string(GetShaderModelProfileName(request.Target)).substr(3);
		configuration += std::format(
		    "{} -T {} -E {} -m {} -O{} -o {} -X \"{}\"\n",
		    producer.Input.generic_string(),
		    profile,
		    request.EntryPoint,
		    shaderModel,
		    request.bEnableOptimizations ? 3 : 0,
		    producer.Name,
		    options);
	}
	return configuration;
}

std::vector<std::string> BuildShaderMakeArguments(const ShaderCompileJob& job, std::string configurationPath, std::string compilerPath, std::uint32_t workers, ShaderMakeOperation operation)
{
	const ShaderCompileRequest& request = job.Request;
	std::vector<std::string> arguments{
	    "-p",
	    IsSpirVTarget(request.Target) ? "SPIRV" : "DXIL",
	    "-c",
	    std::move(configurationPath),
	    "-o",
	    operation == ShaderMakeOperation::Preprocess ? "preprocessed" : "out",
	    "--sourceDir",
	    ".",
	    "--ignoreConfigDir",
	    "--compiler",
	    std::move(compilerPath),
	    "--binary",
	    "--force",
	    "--noRegShifts",
	    "--no16bitTypes",
	    "--noDefaultSpirvExtensions",
	    "--noImplicitDefines",
	    "--relativePaths",
	    "--retryCount",
	    "0",
	    "--jobs",
	    std::to_string(workers),
	    "--vulkanVersion",
	    GetVulkanVersion(request.Target)};
	if (job.BackendName == "slang")
	{
		arguments.insert(arguments.end(), {"--slangProfile", GetSlangTargetProfileName(request.Target), "--matrixRowMajor"});
		const bool bDxilSymbols = IsDxilTarget(request.Target) && request.bCaptureDebugArtifacts;
		if (bDxilSymbols)
		{
			arguments.insert(arguments.end(), {"--outputExt", ".dir"});
		}
	}
	if (operation == ShaderMakeOperation::Preprocess)
	{
		arguments.insert(arguments.end(), {"--outputExt", ".hlsl"});
	}
	return arguments;
}
