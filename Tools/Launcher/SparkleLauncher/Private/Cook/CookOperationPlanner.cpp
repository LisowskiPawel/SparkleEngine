#include "SparkleLauncher/CookOperations.h"

#include "Cook/CookPlanReadiness.h"
#include "Cook/CookOperationProcessRequests.h"

namespace SparkleLauncher
{
	CookOperationPlan PlanLauncherOperation(std::string_view operationId, const CookOperationRequest& request)
	{
		CookOperationPlan plan;
		const CookOperationDefinition* definition = FindCookOperationDefinition(operationId);
		if (definition == nullptr)
		{
			plan.Operation = MakeOperationRecord(std::string(operationId), "Unknown cook operation");
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, "Unknown cook operation id.", "Choose a registered Cook operation, then retry.");
			plan.ReadinessMessages.push_back(plan.Operation.Failure->Summary);
			return plan;
		}

		plan.Kind = definition->Kind;
		plan.Request = request;
		plan.Operation = MakeOperationRecord(definition->Id, definition->DisplayName);
		PrepareCookPlanInputs(plan);
		if (request.Mode == CookMode::Force)
		{
			plan.Operation.DestructiveScope = OperationDestructiveScope::CookedOutputs;
			plan.Operation.bRequiresConfirmation = true;
		}

		const bool bInputsReady = CheckCookInputs(plan);
		const bool bConfirmed = CheckCookConfirmation(plan);
		plan.bCanRun = bInputsReady && bConfirmed;
		if (bInputsReady)
		{
			plan.Steps = BuildCookProcessStepsForPlan(plan);
		}
		return plan;
	}

}
