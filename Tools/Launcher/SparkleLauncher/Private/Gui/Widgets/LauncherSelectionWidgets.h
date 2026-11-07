#pragma once

#include "Gui/Models/LauncherContextUiModel.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <functional>

class QCheckBox;
class QComboBox;
class QObject;
class QWidget;
class QVBoxLayout;

namespace SparkleLauncher
{
	QString PopulateLauncherSelectionCombo(QComboBox& combo, const QVector<LauncherSelectionOption>& options, const QString& preferredValue);

	void ConnectSelectAllScopeBox(QCheckBox* selectAllBox, const QVector<QCheckBox*>& scopeBoxes, QObject* context, std::function<void(bool)> commitSelection);
	QStringList CollectSelectedScopeValues(const QVector<QCheckBox*>& scopeBoxes);
	void AddWorkflowScopeRow(
	    QVBoxLayout& layout,
	    const QString& label,
	    const QString& value,
	    const QString& detail,
	    const QString& metadata,
	    bool bAvailable,
	    const QStringList& selectedScopes,
	    QVector<QCheckBox*>& scopeBoxes,
	    const std::function<void(QWidget*)>& registerFocusable);
	void AddWorkflowAutomationNote(QVBoxLayout& layout, const QString& detail);
}
