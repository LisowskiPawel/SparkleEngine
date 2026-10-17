#pragma once

#include "SparkleLauncher/GpuCrashIncident.h"
#include <optional>

#include <QtCore/QMetaType>
#include <QtCore/QString>

namespace SparkleLauncher
{
	struct LauncherOperationResult final
	{
		QString Status;
		std::optional<GpuCrashIncident> GpuCrash;
		QString ShaderExportPath;
		QString ProblemKind;
		QString Problem;
		QString ExpectedAction;
		QString LogPath;
		int ExitCode = 0;
		bool HasExitCode = false;
		bool Succeeded = false;
		bool Skipped = false;
		bool Canceled = false;
	};
}

Q_DECLARE_METATYPE(SparkleLauncher::LauncherOperationResult)
