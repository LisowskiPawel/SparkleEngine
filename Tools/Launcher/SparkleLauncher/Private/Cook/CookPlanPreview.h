#pragma once

#include "SparkleLauncher/CookOperations.h"

namespace SparkleLauncher
{
	void RecordCookInputs(CookOperationPlan& plan);
	void PopulateCookPreview(CookOperationPlan& plan, std::string_view displayName, bool bInputsReady);
}
