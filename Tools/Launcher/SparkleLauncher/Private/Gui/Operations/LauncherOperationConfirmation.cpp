#include "Gui/Operations/LauncherOperationConfirmation.h"
#include "Gui/Operations/LauncherOperationRequest.h"
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
		const auto plan = PlanLauncherOperation("workspace.clean", *clean);
		if (plan.Operation.Failure)
		{
			QMessageBox::warning(&parent, "Clean targets could not be resolved", QString::fromStdString(plan.Operation.Failure->Summary));
			return false;
		}
		QStringList targets;
		for (const auto& target : plan.CleanTargets)
		{
			targets.push_back(QString::fromStdString(target.DisplayName) + "\n" + QString::fromStdWString(target.Path.wstring()) + "\n" + QString::fromStdString(target.Detail));
		}
		QString message = "Targets to clean:\n\n" + targets.join("\n\n");

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
