#include "Gui/App/LauncherGuiApp.h"

#include "Gui/App/LauncherBackend.h"
#include "Gui/Shell/LauncherMainWindow.h"
#include "Gui/Models/LauncherContentModel.h"
#include "Workspace/LauncherRootResolver.h"
#include "Gui/App/LauncherSettings.h"
#include "Startup/LauncherArguments.h"
#include "SparkleLauncher/WorkspaceLocator.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

#include <filesystem>
#include <functional>
#include <optional>
#include <string>

#if defined(_WIN32)
  #define NOMINMAX
  #ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
  #endif
  #include <Windows.h>
#endif

namespace SparkleLauncher
{
	static void ForceShowWindow(LauncherMainWindow& mainWindow)
	{
		mainWindow.showNormal();
		mainWindow.raise();
		mainWindow.activateWindow();

#if defined(_WIN32)
		const HWND handle = reinterpret_cast<HWND>(mainWindow.winId());
		if (handle != nullptr)
		{
			ShowWindow(handle, SW_SHOWNORMAL);
			SetForegroundWindow(handle);
		}
#endif
	}

	static void CreateLauncherWindow(const std::filesystem::path& workspaceRoot)
	{
		auto* settings = new LauncherSettings();
		auto* contentModel = new LauncherContentModel();
		auto* backend = new LauncherBackend();
		auto* mainWindow = new LauncherMainWindow(workspaceRoot, *contentModel, *settings, *backend);
		const auto showWindow = std::bind_front(ForceShowWindow, std::ref(*mainWindow));
		showWindow();
		QTimer::singleShot(0, mainWindow, showWindow);
		QTimer::singleShot(250, mainWindow, showWindow);

		QObject::connect(mainWindow, &QObject::destroyed, settings, &QObject::deleteLater);
		QObject::connect(mainWindow, &QObject::destroyed, contentModel, &QObject::deleteLater);
		QObject::connect(mainWindow, &QObject::destroyed, backend, &QObject::deleteLater);
	}

	int RunLauncherGui(int argc, char** argv)
	{
		QApplication application(argc, argv);
		QApplication::setApplicationName("Sparkle Launcher");
		QApplication::setOrganizationName("Sparkle Engine");

		std::string repositoryError;

		const auto requestedRoot = ParseLauncherGuiRoot(argc, argv);
		const std::filesystem::path applicationDirectory(QCoreApplication::applicationDirPath().toStdWString());
		const std::optional<std::filesystem::path> workspace = TryResolveLauncherRoot(requestedRoot, applicationDirectory, repositoryError);

		if (!workspace)
		{
			QMessageBox::critical(nullptr, QStringLiteral("Sparkle Launcher"), QString::fromStdString("Launcher startup failed. " + repositoryError));
			return 1;
		}

		const std::filesystem::path workspaceRoot = *workspace;
		std::error_code errorCode;
		std::filesystem::current_path(workspaceRoot, errorCode);
		if (errorCode || !QDir::setCurrent(QString::fromStdWString(workspaceRoot.wstring())))
		{
			QMessageBox::critical(
			    nullptr,
			    QStringLiteral("Sparkle Launcher"),
			    QString::fromStdString("Launcher startup failed. Repository working directory could not be selected: " + workspaceRoot.string()));

			return 1;
		}

		QTimer::singleShot(0, &application, std::bind_front(CreateLauncherWindow, workspaceRoot));

		return QApplication::exec();
	}
}
