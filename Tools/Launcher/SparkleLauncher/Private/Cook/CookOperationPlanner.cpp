#include "SparkleLauncher/CookOperations.h"

#include "Cook/CookPlanReadiness.h"
#include "Cook/CookPlanPreview.h"

namespace SparkleLauncher
{
	CookOperationPlan PlanCookOperation(std::string_view operationId, const CookOperationRequest& request)
	{
		CookOperationPlan plan;
		const std::optional<CookOperationDefinition> definition = FindCookOperationDefinition(operationId);
		if (!definition.has_value())
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
		RecordCookInputs(plan);

		const bool bInputsReady = CheckCookInputs(plan);
		const bool bConfirmed = CheckCookConfirmation(plan);
		plan.bCanRun = bInputsReady && bConfirmed;
		PopulateCookPreview(plan, definition->DisplayName, bInputsReady);
		return plan;
	}

}
