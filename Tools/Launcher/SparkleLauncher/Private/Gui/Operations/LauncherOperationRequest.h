#pragma once

#include "RHI/Public/Diagnostics/ExternalCaptureProvider.h"
#include "RHI/Public/Diagnostics/RhiGpuCrash.h"

#include <QtCore/QString>
#include <QtCore/QVector>

#include <filesystem>

namespace SparkleLauncher
{
	struct LauncherCleanTarget
	{
		QString DisplayName;
		QString Path;
		QString Detail;
	};

	struct LauncherOperationRequest
	{
		std::filesystem::path WorkspaceRoot;
		QString RunId;
		QString OperationId;
		QString ContentId;
		QString RunMode = "editor";
		QString EditorProfile;
		QString GameProfile;
		QString WorkspaceIde;
		QString WorkspaceCompiler;
		QString BuildScopes;
		QString CookScopes;
		QString RequestedLevelIds;
		bool bSelectAvailableLevels = false;
		bool bCleanLevelContent = false;
		QString SourceDependencyId;
		QString HostToolId;
		QString ShaderBackend;
		bool bShaderSymbols = false;
		QString GraphicsApi;
		ExternalCaptureProvider CaptureProvider = ExternalCaptureProvider::None;
		RhiGpuCrashMode GpuCrashMode = RhiGpuCrashMode::Off;
		QString ShaderExportPath;
		QString CleanScope = "cooked";
		QVector<LauncherCleanTarget> CleanTargets;
		QVector<QString> PreservedPaths;
		bool bConfirmClean = false;
	};
}
