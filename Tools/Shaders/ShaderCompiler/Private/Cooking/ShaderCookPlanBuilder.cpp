#include "PCH.h"

#include "Cooking/ShaderCookPlanBuilder.h"

#include "Backend/ShaderBackendPool.h"
#include "Contracts/ShaderContractCatalogBuilder.h"
#include "Contracts/ShaderContractValidator.h"
#include "Cooking/ShaderCompileJobBuilder.h"
#include "Cooking/ShaderCookPlanner.h"
#include "Cooking/ShaderCookSettings.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/FileSystemUtils.h"

#include <algorithm>
#include <format>
#include <span>
#include <unordered_map>
#include <utility>

namespace ShaderCookPlanBuilder
{
	static void ValidateTargets(std::span<const ShaderTarget> targets)
	{
		if (targets.empty())
		{
			throw Diagnostics::Error("Shader cooking requires at least one target.");
		}
		for (std::size_t targetIndex = 0; targetIndex < targets.size(); ++targetIndex)
		{
			if (!IsShaderTarget(targets[targetIndex]) || std::find(targets.begin(), targets.begin() + targetIndex, targets[targetIndex]) != targets.begin() + targetIndex)
			{
				throw Diagnostics::Error("Shader cooking received an invalid or duplicate target.");
			}
		}
	}

	static ShaderDependencyManifest ReadDependencyManifest(const ShaderCookSettings& settings)
	{
		const std::filesystem::path dependencyPath = ShaderDependencyManifest::GetPath(Filesystem::GetCookedShaderRootPath());
		if (!settings.changedVirtualPaths.empty())
		{
			return ShaderDependencyManifest::ReadRequired(dependencyPath);
		}
		if (!settings.shaderId.empty())
		{
			if (std::optional<ShaderDependencyManifest> existing = ShaderDependencyManifest::ReadForUpdate(dependencyPath))
			{
				return std::move(*existing);
			}
		}
		return {};
	}

	static ShaderContractCatalog BuildValidatedCatalog()
	{
		ShaderContractCatalog catalog = ShaderContractCatalogBuilder::Build(ShaderContractSelectionKind::All, {});
		const std::vector<ShaderContractVerificationFailure> contractFailures = ShaderContractValidator::Validate(catalog);
		if (catalog.empty())
		{
			throw Diagnostics::Error("Shader catalog is empty.");
		}
		if (!contractFailures.empty())
		{
			throw Diagnostics::Error("Shader contract invalid " + ShaderContractValidator::FormatFailure(contractFailures.front()));
		}
		return catalog;
	}

	static void SelectShaders(const ShaderCookSettings& settings, const ShaderContractCatalog& catalog, ShaderCookPipelinePlan& plan)
	{
		plan.registeredShaderTypes.reserve(catalog.size());
		for (const ShaderContract& shader : catalog)
		{
			plan.registeredShaderTypes.push_back(shader.shaderTypeId);
		}
		plan.shaders = ShaderCookPlanner::BuildShaders(settings, plan.dependencyManifest, catalog);
		plan.shaderOutputs.resize(plan.shaders.size());
	}

	static void BuildCompileJobs(
	    const ShaderCookSettings& settings,
	    const ShaderContractCatalog& catalog,
	    ShaderBackendPool& backendPool,
	    ShaderCookPipelinePlan& plan,
	    const ToolWorkProgressCallback& progress)
	{
		if (plan.shaders.empty())
		{
			return;
		}
		const ShaderCompileJobBuilder jobBuilder(catalog);
		const std::size_t planningWork = plan.shaders.size() * settings.targets.size();
		std::size_t completedPlanningWork = 0;
		if (progress)
		{
			progress({.Action = "Planning shader compile jobs", .Total = planningWork});
		}
		for (std::size_t shaderIndex = 0; shaderIndex < plan.shaders.size(); ++shaderIndex)
		{
			plan.shaderOutputs[shaderIndex].reserve(settings.targets.size());
			for (const ShaderTarget target : settings.targets)
			{
				ShaderCompileJob job = jobBuilder.Build(settings, plan.shaders[shaderIndex], target, backendPool);
				job.ShaderIndex = shaderIndex;
				plan.jobs.push_back(std::move(job));
				if (progress)
				{
					progress({.Action = "Planned shader", .Item = plan.shaders[shaderIndex].shaderTypeName, .Completed = ++completedPlanningWork, .Total = planningWork});
				}
			}
		}
	}

	static void BuildDependencyManifest(ShaderCookPipelinePlan& plan)
	{
		std::unordered_map<ShaderTypeId, ShaderDependencyRecord> records;
		for (const ShaderCompileJob& job : plan.jobs)
		{
			auto [record, inserted] = records.try_emplace(
			    job.Request.ShaderType,
			    ShaderDependencyRecord{.ShaderType = job.Request.ShaderType, .ShaderTypeName = job.Request.ShaderTypeName, .VirtualSourcePath = job.Request.VirtualSourcePath});

			if (!inserted && (record->second.ShaderTypeName != job.Request.ShaderTypeName || record->second.VirtualSourcePath != job.Request.VirtualSourcePath))
			{
				throw Diagnostics::Error(std::format("Shader type '{}' produced conflicting dependency identities.", job.Request.ShaderTypeName));
			}
			record->second.VirtualDependencies.insert(record->second.VirtualDependencies.end(), job.VirtualDependencies.begin(), job.VirtualDependencies.end());
		}
		for (auto& [shaderType, record] : records)
		{
			(void) shaderType;
			plan.dependencyManifest.Replace(std::move(record));
		}
		plan.removedShaderTypeCount = plan.dependencyManifest.RemoveUnregisteredShaderTypes(plan.registeredShaderTypes);
		plan.dependencyManifest.SortAndValidate();
		if (!plan.dependencyManifest.MatchesShaderTypes(plan.registeredShaderTypes))
		{
			throw Diagnostics::Error(
			    "Shader dependency metadata does not cover the current registered catalog. Run RecompileShaders Global to rebuild all "
			    "shaders.");
		}
	}

	ShaderCookPipelinePlan Build(const ShaderCookSettings& settings, ShaderBackendPool& backendPool, const ToolWorkProgressCallback& progress)
	{
		ValidateTargets(settings.targets);
		ShaderCookPipelinePlan plan;
		plan.dependencyManifest = ReadDependencyManifest(settings);
		const ShaderContractCatalog catalog = BuildValidatedCatalog();
		SelectShaders(settings, catalog, plan);
		BuildCompileJobs(settings, catalog, backendPool, plan, progress);
		BuildDependencyManifest(plan);
		return plan;
	}
}
