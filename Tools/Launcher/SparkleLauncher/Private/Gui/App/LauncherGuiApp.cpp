#include "LauncherGuiApp.h"

#include "LauncherBackend.h"
#include "LauncherMainWindow.h"
#include "LauncherContentModel.h"
#include "LauncherRootResolver.h"
#include "LauncherSettings.h"
#include "LauncherShadowExecution.h"
#include "SparkleLauncher/WorkspaceLocator.h"
#include "Core/Public/Strings/StringUtils.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtGui/QWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

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

	static std::filesystem::path FindRequestedWorkspaceRoot(int argc, char** argv)
	{
		for (int index = 1; index + 1 < argc; ++index)
		{
			if (std::string_view(argv[index]) == "--root")
			{
				return Strings::ToWide(std::string_view(argv[index + 1]));
			}
		}
		return {};
	}

	int RunLauncherGui(int argc, char** argv)
	{
		QApplication application(argc, argv);
		QApplication::setApplicationName("Sparkle Launcher");
		QApplication::setOrganizationName("Sparkle Engine");

		std::string repositoryError;

		const std::optional<std::filesystem::path> workspace = TryResolveLauncherRoot(
		    FindRequestedWorkspaceRoot(argc, argv),
		    std::filesystem::path(QCoreApplication::applicationDirPath().toStdWString()),
		    repositoryError);

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

		const LauncherShadowStartResult shadow = StartLauncherShadow(workspaceRoot, {"--root", workspaceRoot.string()}, LauncherShadowCompletionPolicy::ReleaseCallingArtifact);
		if (shadow.State == LauncherShadowStartState::Started)
		{
			return shadow.ExitCode;
		}
		if (shadow.State == LauncherShadowStartState::Failed)
		{
			QMessageBox::critical(nullptr, QStringLiteral("Sparkle Launcher"), QString::fromStdString(shadow.ErrorMessage));
			return 1;
		}

		QTimer::singleShot(
		    0,
		    &application,
		    [workspaceRoot]()
		    {
			    auto* settings = new LauncherSettings();
			    auto* contentModel = new LauncherContentModel();
			    auto* backend = new LauncherBackend();
			    auto* mainWindow = new LauncherMainWindow(workspaceRoot, *contentModel, *settings, *backend);
			    ForceShowWindow(*mainWindow);
			    QTimer::singleShot(0, mainWindow, [mainWindow]() { ForceShowWindow(*mainWindow); });
			    QTimer::singleShot(250, mainWindow, [mainWindow]() { ForceShowWindow(*mainWindow); });

			    QObject::connect(mainWindow, &QObject::destroyed, settings, &QObject::deleteLater);
			    QObject::connect(mainWindow, &QObject::destroyed, contentModel, &QObject::deleteLater);
			    QObject::connect(mainWindow, &QObject::destroyed, backend, &QObject::deleteLater);
		    });

		return QApplication::exec();
	}
}
