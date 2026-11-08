#pragma once

#include "SparkleLauncher/CookOperations.h"

namespace SparkleLauncher
{
	void PrepareCookPlanInputs(CookOperationPlan& plan);
	bool CheckCookInputs(CookOperationPlan& plan);
	bool CheckCookConfirmation(CookOperationPlan& plan);
}
