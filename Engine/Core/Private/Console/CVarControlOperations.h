#pragma once

#include "Core/Public/Console/CVarControl.h"

#include <string>
#include <string_view>
#include <vector>

class ConsoleVariableBase;

bool ResolveCVarControlVariables(ConsoleVariableRegistry& registry, const CVarControlRequest& request, std::vector<ConsoleVariableBase*>& variables, std::string& outError);
void CommitCVarControlValues(const CVarControlRequest& request, const std::vector<ConsoleVariableBase*>& variables);
void ReadCVarControlValues(const std::vector<ConsoleVariableBase*>& variables, CVarControlResult& result);
