#include "Gui/Shell/LauncherWorkflowPanel.h"

#include "Gui/Style/LauncherIconLibrary.h"
#include "Gui/Style/LauncherUiDesign.h"

#include <QtGui/QColor>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QStyle>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

#include <algorithm>

namespace SparkleLauncher
{
	LauncherWorkflowPanel::LauncherWorkflowPanel(
	    const LauncherIconLibrary& icons,
	    const std::function<void(QWidget*)>& registerFocusable,
	    const std::function<QWidget*(QWidget*)>& createOptionsPanel,
	    QWidget* parent) :
	    QWidget(parent)
	{
		setObjectName("WorkflowSurface");
		QHBoxLayout* surfaceLayout = new QHBoxLayout(this);
		surfaceLayout->setContentsMargins(0, 0, 0, 0);
		surfaceLayout->setSpacing(0);

		QFrame* rail = new QFrame(this);
		rail->setObjectName("ProcessPanel");
		rail->setFixedWidth(LauncherUi::Shell::RailWidth);
		QVBoxLayout* railLayout = new QVBoxLayout(rail);
		railLayout->setContentsMargins(0, LauncherUi::Shell::RailTopPadding, 0, LauncherUi::Shell::RailBottomPadding);
		railLayout->setSpacing(LauncherUi::Space::XSmall);

		QVBoxLayout* groupLayout = new QVBoxLayout();
		groupLayout->setContentsMargins(0, 0, 0, 0);
		groupLayout->setSpacing(LauncherUi::Shell::RailGroupSpacing);
		m_workflowGroupButtons = new QButtonGroup(this);
		m_workflowGroupButtons->setExclusive(true);
		for (const LauncherWorkflowDefinition& workflow : CreateLauncherWorkflowCatalog())
		{
			QToolButton* groupButton = new QToolButton(rail);
			groupButton->setText(workflow.DisplayName);
			groupButton->setObjectName("WorkflowGroupButton");
			groupButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
			groupButton->setFixedSize(LauncherUi::Shell::RailWidth, LauncherUi::Shell::RailItemMinHeight);
			groupButton->setProperty("OperationId", workflow.OperationId);
			groupButton->setProperty("ActiveState", "false");
			groupButton->setAccessibleName(workflow.DisplayName + " workflow group");
			groupButton->setIcon(icons.Icon(workflow.Icon, QColor(LauncherUi::Color::StateQueued)));
			groupButton->setIconSize(QSize(LauncherUi::Shell::RailIconSize, LauncherUi::Shell::RailIconSize));
			registerFocusable(groupButton);
			m_workflowGroupButtons->addButton(groupButton);
			groupLayout->addWidget(groupButton);
		}
		groupLayout->addStretch(1);
		railLayout->addLayout(groupLayout, 1);

		connect(m_workflowGroupButtons, &QButtonGroup::buttonClicked, this, [this](QAbstractButton* button) { emit OperationSelected(button->property("OperationId").toString()); });

		surfaceLayout->addWidget(rail, 0);
		surfaceLayout->addWidget(createOptionsPanel(this), 1);
	}

	void LauncherWorkflowPanel::SetSelectedOperation(const QString& operationId)
	{
		const auto buttons = m_workflowGroupButtons->buttons();
		const bool bKnownWorkflow = std::any_of(buttons.begin(), buttons.end(), [&operationId](const QAbstractButton* button) { return button->property("OperationId").toString() == operationId; });
		if (!bKnownWorkflow)
		{
			return;
		}
		for (QAbstractButton* button : buttons)
		{
			const bool bActive = button->property("OperationId").toString() == operationId;
			button->setProperty("ActiveState", bActive ? "true" : "false");
			button->style()->unpolish(button);
			button->style()->polish(button);
			button->update();
		}
	}
}
