#pragma once

#include <QtCore/QString>

class QLabel;
class QPushButton;

namespace SparkleLauncher
{
	void ApplyInlineStatusLabel(QLabel& label, const QString& text, const QString& state);
	void ApplyStatusActionButtonPresentation(QPushButton& button, const QString& label, const QString& state);

	QString PrimaryActionLabelForOperationId(const QString& operationId);
	void ApplyActionAvailability(QPushButton& button, bool bEnabled, const QString& reason);
}
