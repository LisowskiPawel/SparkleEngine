#include "SparkleLauncher/MaintenanceOperations.h"

#include "Maintenance/MaintenanceCleanPreview.h"
#include "Maintenance/MaintenanceCleanSteps.h"
#include "Workspace/LauncherProductPaths.h"
#include "Core/Public/Diagnostics/Error.h"

#include <algorithm>

namespace SparkleLauncher
{
	static OperationDestructiveScope ToOperationDestructiveScope(CleanScope scope)
	{
		switch (scope)
		{
			case CleanScope::CookedOutputs:
				return OperationDestructiveScope::CookedOutputs;
			case CleanScope::BuildTree:
				return OperationDestructiveScope::BuildTree;
			case CleanScope::ArtifactOutputs:
				return OperationDestructiveScope::ArtifactOutputs;
			case CleanScope::WorkspaceState:
				return OperationDestructiveScope::WorkspaceState;
			case CleanScope::ThirdPartyDependencyCache:
				return OperationDestructiveScope::DependencyCache;
			case CleanScope::Logs:
				return OperationDestructiveScope::Logs;
			case CleanScope::PristineGeneratedWorkspace:
				return OperationDestructiveScope::PristineGeneratedWorkspace;
		}

		return OperationDestructiveScope::None;
	}

	static bool PrepareMaintenanceRequest(MaintenanceOperationPlan& plan)
	{
		if (!HasBuildSources(plan.Request.WorkspaceRoot))
		{
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, "Source workspace operations are unavailable in a portable package.", "Use a source checkout for this operation.");
			plan.ReadinessMessages.push_back(plan.Operation.Failure->Summary);
			return false;
		}
		const auto scopes = ResolveRequestedCleanScopes(plan.Request);
		const bool bScopeRequiresContent = std::any_of(scopes.begin(), scopes.end(), CleanScopeRequiresContent);
		const bool bNeedsProjectScope = plan.Request.RequestedCleanTargets.empty() && bScopeRequiresContent;
		const bool bRequiresContent = plan.Request.bCleanLevelContent || bNeedsProjectScope;
		if (bRequiresContent && plan.Request.ContentId.empty())
		{
			SetOperationFailure(
			    plan.Operation,
			    OperationProblemKind::Prerequisite,
			    "The selected clean scope requires a content project.",
			    "Select a content project or remove project-owned outputs from the clean request, then retry.");

			plan.ReadinessMessages.push_back(plan.Operation.Failure->Summary);
			return false;
		}
		if (plan.Request.bCleanLevelContent)
		{
			plan.Request.RequestedCleanTargets = BuildLevelCleanPaths(ResolveProjectRoot(plan.Request.WorkspaceRoot, plan.Request.ContentId), {});
		}
		return true;
	}

	static void AdmitMaintenanceClean(MaintenanceOperationPlan& plan)
	{
		const auto scopes = ResolveRequestedCleanScopes(plan.Request);
		const bool bScopeClean = !plan.Request.bCleanLevelContent && plan.Request.RequestedCleanTargets.empty();
		if (bScopeClean && scopes.size() == 1)
		{
			plan.Operation.DestructiveScope = ToOperationDestructiveScope(scopes.front());
		}
		plan.Operation.bRequiresConfirmation = true;
		plan.ReadinessMessages.push_back(plan.Request.bDestructiveActionConfirmed ? "Clean scope was confirmed." : "Clean scope requires explicit confirmation.");
		plan.bCanRun = plan.Request.bDestructiveActionConfirmed;
	}

	MaintenanceOperationPlan PlanLauncherOperation(std::string_view operationId, const MaintenanceOperationRequest& request)
	{
		MaintenanceOperationPlan plan;
		const auto definition = FindMaintenanceOperationDefinition(operationId);
		if (definition == nullptr)
		{
			plan.Operation = MakeOperationRecord(std::string(operationId), "Unknown maintenance operation");
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, "Unknown maintenance operation id.", "Choose a registered Clean operation, then retry.");
			plan.ReadinessMessages.push_back(plan.Operation.Failure->Summary);
			return plan;
		}
		plan.Request = request;
		plan.Operation = MakeOperationRecord(definition->Id, definition->DisplayName);
		try
		{
			if (!PrepareMaintenanceRequest(plan))
			{
				return plan;
			}
			InspectMaintenanceCleanTargets(plan);
			AdmitMaintenanceClean(plan);
		}
		catch (const Diagnostics::Error& error)
		{
			plan.bCanRun = false;
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, error.what(), "Refresh the catalog and clean preview, then retry.");
			plan.ReadinessMessages.push_back(plan.Operation.Failure->Summary);
		}
		return plan;
	}
}
