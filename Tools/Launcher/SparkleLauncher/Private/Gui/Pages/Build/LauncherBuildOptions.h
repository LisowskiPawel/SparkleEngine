#pragma once

#include <QtCore/QString>
#include <filesystem>
#include <functional>

class QVBoxLayout;
class QWidget;

namespace SparkleLauncher
{
	class LauncherSettings;
	void BuildLauncherBuildOptions(QVBoxLayout& layout, LauncherSettings& settings, const std::function<void(QWidget*)>& registerFocusable, const std::function<void()>& selectionChanged);
}
