#pragma once

#include "SparkleLauncher/GpuCrashIncident.h"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>

#include <array>
#include <filesystem>
#include <optional>

class QCheckBox;
class QLabel;
class QPushButton;
class QToolButton;

namespace SparkleLauncher
{
	class LauncherBackend;
	class LauncherSettings;

	class LauncherGpuCrashPanel final : public QWidget
	{
	public:
		LauncherGpuCrashPanel(std::filesystem::path workspaceRoot, LauncherSettings& settings, LauncherBackend& backend, QWidget* parent);
		void SetIncident(GpuCrashIncident incident);
		std::array<QWidget*, 2> GetFocusableWidgets() const noexcept;

	private:
		void OpenGpuCrashTools();
		void OpenGpuCrashInspector(QPushButton& button);
		void RefreshGpuCrashIncident();
		void DisplayGpuCrashState(const QString& state);

		std::filesystem::path m_workspaceRoot;
		LauncherSettings& m_settings;
		LauncherBackend& m_backend;
		std::optional<GpuCrashIncident> m_gpuCrashIncident;
		QPointer<QLabel> m_gpuCrashStatusLabel;
		QCheckBox* m_enabled = nullptr;
		QToolButton* m_tools = nullptr;
		bool m_bGpuCrashQueryPending = false;
	};
}
