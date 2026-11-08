#include "SparkleLauncher/MaintenanceOperations.h"

#include <algorithm>

namespace SparkleLauncher
{
	std::string ToString(CleanScope scope)
	{
		return std::string(CleanScopeId(scope));
	}

	std::string_view CleanScopeId(CleanScope scope) noexcept
	{
		switch (scope)
		{
			case CleanScope::CookedOutputs:
				return "cooked";
			case CleanScope::BuildTree:
				return "build-tree";
			case CleanScope::ArtifactOutputs:
				return "artifacts";
			case CleanScope::WorkspaceState:
				return "workspace-state";
			case CleanScope::ThirdPartyDependencyCache:
				return "deps";
			case CleanScope::Logs:
				return "logs";
			case CleanScope::PristineGeneratedWorkspace:
				return "clean-all";
		}

		return "unknown";
	}

	bool CleanScopeRequiresContent(CleanScope scope) noexcept
	{
		switch (scope)
		{
			case CleanScope::CookedOutputs:
			case CleanScope::BuildTree:
			case CleanScope::WorkspaceState:
			case CleanScope::Logs:
			case CleanScope::PristineGeneratedWorkspace:
				return true;
			case CleanScope::ArtifactOutputs:
			case CleanScope::ThirdPartyDependencyCache:
				return false;
		}
		return false;
	}

	bool TryParseCleanScope(std::string_view text, CleanScope& outScope) noexcept
	{
		static constexpr CleanScope scopes[] = {
		    CleanScope::CookedOutputs,
		    CleanScope::BuildTree,
		    CleanScope::ArtifactOutputs,
		    CleanScope::WorkspaceState,
		    CleanScope::ThirdPartyDependencyCache,
		    CleanScope::Logs,
		    CleanScope::PristineGeneratedWorkspace};

		for (const CleanScope scope : scopes)
		{
			if (CleanScopeId(scope) == text)
			{
				outScope = scope;
				return true;
			}
		}
		return false;
	}

	const std::vector<MaintenanceOperationDefinition>& GetMaintenanceOperationDefinitions()
	{
		static const std::vector<MaintenanceOperationDefinition> definitions = {
		    {"workspace.clean", "Clean", "Clean Workspace", "Remove generated files for the selected confirmed scope."},
		};

		return definitions;
	}

	std::optional<MaintenanceOperationDefinition> FindMaintenanceOperationDefinition(std::string_view operationId)
	{
		const std::vector<MaintenanceOperationDefinition>& definitions = GetMaintenanceOperationDefinitions();
		const auto found = std::find_if(definitions.begin(), definitions.end(), [operationId](const MaintenanceOperationDefinition& definition) { return definition.Id == operationId; });
		return found == definitions.end() ? std::nullopt : std::optional<MaintenanceOperationDefinition>(*found);
	}

}
