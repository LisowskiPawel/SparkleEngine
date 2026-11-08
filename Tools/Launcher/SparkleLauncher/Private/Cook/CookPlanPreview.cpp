#include "Cook/CookPlanPreview.h"

#include "Cook/CookOperationProcessRequests.h"
#include "Workspace/LauncherStatePaths.h"

#include <sstream>
#include <span>
#include <utility>

namespace SparkleLauncher
{
	static void AddPlannedEffect(CookOperationPlan& plan, std::string message)
	{
		plan.PlannedEffects.push_back(std::move(message));
	}

	static std::string JoinCookScopeNames(std::span<const CookWorkspaceScope> scopes, std::string (*describeScope)(CookWorkspaceScope))
	{
		std::string names;
		for (const CookWorkspaceScope scope : scopes)
		{
			if (!names.empty())
			{
				names += ", ";
			}
			names += describeScope(scope);
		}
		return names;
	}

	static void PopulateCookEffects(CookOperationPlan& plan)
	{
		if (plan.Request.Mode == CookMode::Force)
		{
			AddPlannedEffect(plan, "Remove cooked outputs before cooking: " + plan.CookedOutputDirectory.string());
		}

		switch (plan.Kind)
		{
			case CookOperationKind::CookWorkspace:
			{
				const std::string scopeNames = JoinCookScopeNames(plan.Request.SelectedScopes, DisplayName);
				if (!scopeNames.empty())
				{
					AddPlannedEffect(plan, "Cook selected outputs: " + scopeNames + ".");
				}
				if (IncludesCookScope(plan, CookWorkspaceScope::Shaders))
				{
					AddPlannedEffect(plan, "Shader backend: " + plan.Request.ShaderBackend + ".");
					AddPlannedEffect(plan, "Cook the global shader map and code library for the DXIL and SPIR-V targets.");
				}
				return;
			}
			case CookOperationKind::CookShaders:
				AddPlannedEffect(plan, "Shader backend: " + plan.Request.ShaderBackend + ".");
				AddPlannedEffect(plan, std::string("Shader debug info: ") + (plan.Request.bShaderEnableDebugInfo ? "enabled." : "disabled."));
				AddPlannedEffect(plan, std::string("Shader optimizations: ") + (plan.Request.bShaderEnableOptimizations ? "enabled." : "disabled."));
				AddPlannedEffect(plan, std::string("Warnings as errors: ") + (plan.Request.bShaderWarningsAsErrors ? "enabled." : "disabled."));
				AddPlannedEffect(plan, std::string("Strip debug info: ") + (plan.Request.bShaderStripDebugInfo ? "enabled." : "disabled."));
				AddPlannedEffect(plan, "Cook the global shader map and code library for the DXIL and SPIR-V targets.");
				return;
			case CookOperationKind::BuildTextures:
				AddPlannedEffect(plan, "Cook texture assets for " + plan.Request.ContentId + ".");
				return;
			case CookOperationKind::BuildSceneAssets:
				AddPlannedEffect(plan, "Cook scene, mesh, and material assets for " + plan.Request.ContentId + ".");
				return;
			case CookOperationKind::CookAllAssets:
				AddPlannedEffect(plan, "Shader phase backend: " + plan.Request.ShaderBackend + ".");
				AddPlannedEffect(plan, std::string("Shader phase debug info: ") + (plan.Request.bShaderEnableDebugInfo ? "enabled." : "disabled."));
				AddPlannedEffect(plan, std::string("Shader phase optimizations: ") + (plan.Request.bShaderEnableOptimizations ? "enabled." : "disabled."));
				AddPlannedEffect(plan, "Cook all selected level assets.");
				return;
		}
	}

	static void PopulateCookSteps(CookOperationPlan& plan)
	{
		const std::vector<CookOperationProcessStep> processSteps = BuildCookProcessStepsForPlan(plan);
		for (const CookOperationProcessStep& processStep : processSteps)
		{
			CookOperationStep step;
			step.Id = processStep.Id;
			step.DisplayName = processStep.DisplayName;
			step.bDestructive = processStep.bDeletesCookedOutputs;
			step.DestructivePath = processStep.DestructivePath;
			if (processStep.bHasProcessRequest)
			{
				step.DisplayCommandLine = BuildDisplayCommandLine(processStep.Request.ExecutablePath, processStep.Request.Arguments);
				step.LogPath = processStep.Request.LogPath;
			}
			else
			{
				step.DisplayCommandLine = "Delete " + processStep.DestructivePath.string();
			}
			plan.Steps.push_back(std::move(step));
		}
	}

	void RecordCookInputs(CookOperationPlan& plan)
	{
		const CookOperationRequest& request = plan.Request;
		plan.Operation.Inputs.push_back({"content", request.ContentId});
		plan.Operation.Inputs.push_back({"gameProfile", request.GameProfile});
		plan.Operation.Inputs.push_back({"cookMode", ToString(request.Mode)});
		plan.Operation.Inputs.push_back({"toolProfile", plan.ToolProfile});
		plan.Operation.Inputs.push_back({"shaderBackend", request.ShaderBackend});
		if (plan.Kind == CookOperationKind::CookWorkspace && !plan.Request.SelectedScopes.empty())
		{
			plan.Operation.Inputs.push_back({"cookScopes", JoinCookScopeNames(plan.Request.SelectedScopes, ToString)});
		}
		plan.Operation.LogPath = ResolveLauncherOperationLogPath(request.WorkspaceRoot, plan.Operation.Id, "Latest.txt");
		if (request.Mode == CookMode::Force)
		{
			plan.Operation.DestructiveScope = OperationDestructiveScope::CookedOutputs;
			plan.Operation.bRequiresConfirmation = true;
		}
	}

	static void DescribeCookDryRun(CookOperationPlan& plan, std::string_view displayName)
	{
		std::ostringstream dryRun;
		dryRun << "Dry-run plan for " << displayName << ":";
		for (const CookOperationStep& step : plan.Steps)
		{
			dryRun << "\n  " << step.DisplayName << ": " << step.DisplayCommandLine;
			if (!step.LogPath.empty())
			{
				dryRun << "\n    Log: " << step.LogPath.string();
			}
		}
		if (plan.Steps.empty())
		{
			dryRun << "\n  No command step available until readiness issues are resolved.";
		}
		plan.Operation.DryRunText = dryRun.str();
	}

	void PopulateCookPreview(CookOperationPlan& plan, std::string_view displayName, bool bInputsReady)
	{
		PopulateCookEffects(plan);
		if (bInputsReady)
		{
			PopulateCookSteps(plan);
		}
		DescribeCookDryRun(plan, displayName);
	}
}
