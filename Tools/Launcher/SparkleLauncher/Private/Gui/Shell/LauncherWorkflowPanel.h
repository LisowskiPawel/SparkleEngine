#pragma once

#include "Gui/Models/LauncherWorkflowCatalog.h"

#include <QtWidgets/QWidget>

#include <functional>

class QButtonGroup;

namespace SparkleLauncher
{
	class LauncherIconLibrary;

	class LauncherWorkflowPanel final : public QWidget
	{
		Q_OBJECT

	public:
		LauncherWorkflowPanel(
		    const LauncherIconLibrary& icons,
		    const std::function<void(QWidget*)>& registerFocusable,
		    const std::function<QWidget*(QWidget*)>& createOptionsPanel,
		    QWidget* parent = nullptr);

		void SetSelectedOperation(const QString& operationId);

	signals:
		void OperationSelected(const QString& operationId);

	private:
		QButtonGroup* m_workflowGroupButtons = nullptr;
	};
}
