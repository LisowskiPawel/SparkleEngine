#include "Gui/Operations/LauncherOperationConfirmation.h"
#include "Gui/Operations/LauncherOperationRequest.h"
#include "Gui/Models/LauncherCleanUiModel.h"
#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtWidgets/QMessageBox>

namespace SparkleLauncher
{
	bool ConfirmLauncherRunRequest(QWidget& parent, LauncherOperationRequest& request)
	{
		if (request.OperationId != "workspace.clean" || request.bConfirmClean)
		{
			return true;
		}

		const bool bCustomCleanRequested = !request.CleanTargets.isEmpty();
		QStringList scopeNames;
		if (bCustomCleanRequested)
		{
			for (const LauncherCleanTarget& target : request.CleanTargets)
			{
				scopeNames.push_back(target.DisplayName + "\n" + target.Path);
			}
		}
		else
		{
			for (const QString& scopeValue : request.CleanScope.split(QRegularExpression("[,;\\n]"), Qt::SkipEmptyParts))
			{
				scopeNames.push_back(CleanScopeDisplayName(scopeValue));
			}
		}

		QString message = bCustomCleanRequested ? "Generated outputs to clean:\n\n" + scopeNames.join("\n\n") : "Clean scopes:\n" + scopeNames.join('\n');

		message += bCustomCleanRequested ? "\n\nThis removes only the generated outputs mapped to the selected action. Continue?"
		                                 : "\n\nThis removes generated files for the selected scope. Continue?";

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

		request.bConfirmClean = true;
		return true;
	}
}
