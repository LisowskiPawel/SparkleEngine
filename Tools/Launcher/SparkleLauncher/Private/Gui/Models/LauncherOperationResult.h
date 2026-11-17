#pragma once

#include "SparkleLauncher/OperationModel.h"
#include <optional>

#include <QtCore/QMetaType>
#include <QtCore/QString>

namespace SparkleLauncher
{
	struct LauncherOperationResult final
	{
		OperationStatus Status = OperationStatus::Pending;
		std::optional<GpuCrashIncident> GpuCrash;
		QString ShaderExportPath;
		QString ProblemKind;
		QString Problem;
		QString ExpectedAction;
		QString LogPath;
		std::optional<int> ExitCode;
	};
}

Q_DECLARE_METATYPE(SparkleLauncher::LauncherOperationResult)
