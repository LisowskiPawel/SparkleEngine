#include "Gui/Widgets/LauncherActionWidgets.h"

#include "Gui/Style/LauncherUiDesign.h"
#include "Gui/Models/LauncherUiModel.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>

namespace SparkleLauncher
{
	void ApplyInlineStatusLabel(QLabel& label, const QString& text, const QString& state)
	{
		label.setObjectName("InlineStatusValue");
		label.setFixedWidth(LauncherUi::Row::InlineStatusWidth);
		label.setText(QStringLiteral("● %1").arg(text));
		label.setProperty("State", state);
		label.style()->unpolish(&label);
		label.style()->polish(&label);
	}

	void ApplyStatusActionButtonPresentation(QPushButton& button, const QString& label, const QString& state)
	{
		button.setObjectName("StatusActionButton");
		button.setFixedSize(LauncherUi::Row::StatusActionWidth, LauncherUi::Row::StatusActionHeight);
		button.setText(label);
		button.setProperty("ActionState", state);
		button.style()->unpolish(&button);
		button.style()->polish(&button);
	}

	QString PrimaryActionLabelForOperationId(const QString& operationId)
	{
		const QString primaryVerb = LauncherUiModelForOperation(operationId).PrimaryVerb;
		return primaryVerb.isEmpty() ? QString("Run") : primaryVerb;
	}

	void ApplyActionAvailability(QPushButton& button, bool bEnabled, const QString& reason)
	{
		button.setEnabled(bEnabled);
		button.setToolTip(reason);
		button.setAccessibleDescription(reason);
	}
}
