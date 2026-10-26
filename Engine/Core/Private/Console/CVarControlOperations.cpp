#include "PCH.h"

#include "Console/CVarControlOperations.h"
#include "Core/Public/Console/CVar.h"
#include "Core/Public/Console/CVarRegistry.h"
#include "Core/Public/Strings/StringUtils.h"

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogCoreCVarControl, "Core.Console.Control");

static bool CollectFilteredCVars(ConsoleVariableRegistry& registry, const CVarControlRequest& request, std::vector<ConsoleVariableBase*>& variables, std::string& outError)
{
	if (!request.Entries.empty())
	{
		outError = "List does not accept entries.";
		return false;
	}
	for (ConsoleVariableBase* variable : registry.GetVariables())
	{
		if (variable != nullptr
		    && (request.Filter.empty() || Strings::ContainsIgnoreCase(variable->GetName(), request.Filter) || Strings::ContainsIgnoreCase(variable->GetDescription(), request.Filter)))
		{
			variables.push_back(variable);
		}
	}
	return true;
}

static bool ResolveNamedCVarBatch(ConsoleVariableRegistry& registry, const CVarControlRequest& request, std::vector<ConsoleVariableBase*>& variables, std::string& outError)
{
	constexpr std::size_t maximumBatchSize = 64;
	if (request.Operation != CVarControlOperation::Query && request.Operation != CVarControlOperation::Set)
	{
		outError = "Invalid CVar operation.";
		return false;
	}
	if (request.Entries.empty() || request.Entries.size() > maximumBatchSize || !request.Filter.empty())
	{
		outError = "Query/Set requires 1 through 64 entries and no filter.";
		return false;
	}
	variables.reserve(request.Entries.size());
	for (const CVarControlEntry& entry : request.Entries)
	{
		ConsoleVariableBase* variable = registry.Find(entry.Name);
		if (variable == nullptr)
		{
			outError = "unknown CVar: " + entry.Name;
			return false;
		}
		for (const ConsoleVariableBase* previous : variables)
		{
			if (previous == variable)
			{
				outError = "duplicate CVar: " + entry.Name;
				return false;
			}
		}
		if (request.Operation == CVarControlOperation::Set)
		{
			std::string error;
			if (!variable->ValidateValueFromString(entry.Value, error))
			{
				outError = "failed to set " + entry.Name + ": " + error;
				return false;
			}
		}
		else if (!entry.Value.empty())
		{
			outError = "Query does not accept values.";
			return false;
		}
		variables.push_back(variable);
	}
	return true;
}

bool ResolveCVarControlVariables(ConsoleVariableRegistry& registry, const CVarControlRequest& request, std::vector<ConsoleVariableBase*>& variables, std::string& outError)
{
	return request.Operation == CVarControlOperation::List ? CollectFilteredCVars(registry, request, variables, outError) : ResolveNamedCVarBatch(registry, request, variables, outError);
}

void CommitCVarControlValues(const CVarControlRequest& request, const std::vector<ConsoleVariableBase*>& variables)
{
	// Validation and assignment use the same pure parser and immutable text.
	// The host executes this entire operation between frames.
	for (std::size_t index = 0; index < variables.size(); ++index)
	{
		std::string error;
		if (!variables[index]->TrySetValueFromString(request.Entries[index].Value, error))
		{
			Diagnostics::Fatal(LogCoreCVarControl, __FILE__, __LINE__, "Previously validated CVar input failed during commit: " + error);
		}
	}
}

void ReadCVarControlValues(const std::vector<ConsoleVariableBase*>& variables, CVarControlResult& result)
{
	result.Values.reserve(variables.size());
	for (const ConsoleVariableBase* variable : variables)
	{
		result.Values.push_back({std::string(variable->GetName()), variable->GetValueAsString(), variable->GetValueTypeName(), std::string(variable->GetDescription())});
	}
}
