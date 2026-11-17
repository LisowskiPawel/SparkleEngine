#pragma once

#include <QtCore/QString>
#include <functional>

class QVBoxLayout;
class QWidget;

namespace SparkleLauncher
{
	class LauncherSettings;
	void BuildLauncherCookOptions(QVBoxLayout& layout, LauncherSettings& settings, const std::function<void(QWidget*)>& registerFocusable, const std::function<void()>& selectionChanged);
}
