#pragma once

#include <QtCore/QString>

class QLabel;
class QVBoxLayout;
class QWidget;

namespace SparkleLauncher
{
	QVBoxLayout* AddOptionGroup(QVBoxLayout& layout, const QString& title, const QString& detail);
	void AddNoOptionsMessage(QVBoxLayout& layout, const QString& text);
	QLabel* AddStatusRow(QVBoxLayout& layout, const QString& label, const QString& status, const QString& detail, const QString& state, QWidget* accessory = nullptr);
	QLabel* CreateSectionLabel(const QString& title);
	QLabel* CreateFieldLabel(const QString& title);
}
