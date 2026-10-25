#include "PCH.h"

#include "Cooking/GlobalShaderCooker.h"

#include "Backend/ShaderBackendPool.h"
#include "Cooking/ShaderArtifactPublication.h"
#include "Cooking/ShaderCompileBatch.h"
#include "Cooking/ShaderCookCancellation.h"
#include "Cooking/ShaderCookPlanBuilder.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/FileSystemUtils.h"

#include <span>

ShaderCookResult CookGlobalShaders(const ShaderCookSettings& settings, const ToolWorkProgressCallback& progress)
{
	ShaderCookResult result;
	result.outputDirectory = Filesystem::GetCookedShaderRootPath();
	if (progress)
	{
		progress({.Action = "Planning shader cook"});
	}
	ShaderBackendPool backendPool;
	ShaderCookPipelinePlan plan = BuildShaderCookPlan(settings, backendPool, progress);
	result.compileJobCount = plan.jobs.size();
	result.selectedShaderCount = plan.shaders.size();
	if (plan.jobs.empty() && plan.removedShaderTypeCount == 0)
	{
		if (progress)
		{
			progress({.Action = "Shader cook is up to date", .Completed = 1, .Total = 1});
		}
		return result;
	}
	const std::size_t totalWork = plan.jobs.size() * 2 + 1;
	if (progress)
	{
		progress({.Action = plan.jobs.empty() ? "Preparing shader publication" : "Compiling shaders", .Total = totalWork});
	}

	std::vector<ShaderCompileResult> compileResults;
	if (!plan.jobs.empty())
	{
		compileResults = CompileShaderBatch(settings, plan.jobs, totalWork, progress);
	}
	if (progress)
	{
		progress({.Action = "Preparing shader publication", .Completed = totalWork - 1, .Total = totalWork});
	}
	PrepareShaderProducts(plan, compileResults);

	ThrowIfShaderCookCancelled(settings.cancellationSignalPath);
	if (progress)
	{
		progress({.Action = "Publishing cooked shaders", .Completed = totalWork - 1, .Total = totalWork});
	}
	result.output = PublishShaderArtifacts(plan, compileResults, settings, result.outputDirectory);
	if (progress)
	{
		progress({.Action = "Cooked shaders published", .Completed = totalWork, .Total = totalWork});
	}
	return result;
}
