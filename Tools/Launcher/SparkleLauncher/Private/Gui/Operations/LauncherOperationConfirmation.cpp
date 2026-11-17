#include "Gui/Operations/LauncherOperationConfirmation.h"
#include "Gui/Operations/LauncherOperationRequest.h"
#include "Gui/Models/LauncherCleanUiModel.h"
#include <QtCore/QStringList>
#include <QtWidgets/QMessageBox>

namespace SparkleLauncher
{
	bool ConfirmLauncherRunRequest(QWidget& parent, LauncherOperationRequest& request)
	{
		auto* clean = std::get_if<MaintenanceOperationRequest>(&request.Inputs);
		if (clean == nullptr || clean->bDestructiveActionConfirmed)
		{
			return true;
		}

		const bool bCustomCleanRequested = clean->bCleanLevelContent || !clean->RequestedCleanTargets.empty();
		QStringList scopeNames;
		if (clean->bCleanLevelContent)
		{
			const auto plan = PlanMaintenanceOperation("workspace.clean", *clean);
			if (plan.Operation.Failure.has_value())
			{
				QMessageBox::warning(&parent, "Level content could not be resolved", QString::fromStdString(plan.Operation.Failure->Summary));
				return false;
			}
			for (const auto& target : plan.CleanTargets)
			{
				scopeNames.push_back(QString::fromStdString(target.DisplayName) + "\n" + QString::fromStdString(target.Path.string()));
			}
		}
		else if (bCustomCleanRequested)
		{
			for (const MaintenanceCleanPathSpec& target : clean->RequestedCleanTargets)
			{
				scopeNames.push_back(QString::fromStdString(target.DisplayName) + "\n" + QString::fromStdWString(target.Path.wstring()));
			}
		}
		else
		{
			for (const CleanScope scope : clean->RequestedCleanScopes)
			{
				scopeNames.push_back(CleanScopeDisplayName(QString::fromUtf8(CleanScopeId(scope))));
			}
		}

		QString message = bCustomCleanRequested ? "Generated outputs to clean:\n\n" + scopeNames.join("\n\n") : "Clean scopes:\n" + scopeNames.join('\n');

		message += bCustomCleanRequested ? "\n\nThis removes only the generated outputs mapped to the selected action. Continue?"
		                                 : "\n\nThis removes generated files for the selected scope. Continue?";

		if (clean->bCleanLevelContent)
		{
			message += "\n\nThis also disables catalog selection for all levels except Empty.";
		}

		const QMessageBox::StandardButton result = QMessageBox::question(
		    &parent,
		    bCustomCleanRequested ? "Confirm Action Clean" : "Confirm Clean Workspace",
		    message,
		    QMessageBox::Ok | QMessageBox::Cancel,
		    QMessageBox::Cancel);

		if (result != QMessageBox::Ok)
		{
			return false;
		}

		clean->bDestructiveActionConfirmed = true;
		return true;
	}
}
