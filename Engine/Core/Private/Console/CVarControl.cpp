#include "PCH.h"

#include "Core/Public/Console/CVarControl.h"
#include "Console/CVarControlOperations.h"

CVarControlResult ExecuteCVarControl(ConsoleVariableRegistry& registry, CVarControlRequest request)
{
	CVarControlResult result;
	std::vector<ConsoleVariableBase*> variables;
	if (!ResolveCVarControlVariables(registry, request, variables, result.Error))
	{
		return result;
	}
	if (request.Operation == CVarControlOperation::Set)
	{
		CommitCVarControlValues(request, variables);
	}
	ReadCVarControlValues(variables, result);
	return result;
}
