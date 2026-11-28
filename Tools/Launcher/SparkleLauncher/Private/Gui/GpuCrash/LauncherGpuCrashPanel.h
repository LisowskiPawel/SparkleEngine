#pragma once

#include <QtWidgets/QWidget>

#include <array>

class QCheckBox;
class QToolButton;

namespace SparkleLauncher
{
	class LauncherSettings;

	class LauncherGpuCrashPanel final : public QWidget
	{
	public:
		LauncherGpuCrashPanel(bool bHasBuildSources, LauncherSettings& settings, QWidget* parent);
		std::array<QWidget*, 2> GetFocusableWidgets() const noexcept;

	private:
		void ConfigureGpuCrashTools();
		void LaunchConfiguredGpuDebugger();

		LauncherSettings& m_settings;
		QCheckBox* m_enabled = nullptr;
		QToolButton* m_tools = nullptr;
	};
}
