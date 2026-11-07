#include "Gui/Models/LauncherOperationRequestFactory.h"

#include "Gui/Models/LauncherCleanUiModel.h"
#include "Gui/Models/LauncherDependencyUiModel.h"
#include "Gui/Models/LauncherContentModel.h"
#include "Gui/App/LauncherSettings.h"

#include <QtCore/QRegularExpression>

#include <algorithm>

namespace SparkleLauncher
{
	static std::vector<BuildWorkspaceScope> ResolveSelectedBuildScopes(const LauncherSettings& settings)
	{
		std::vector<BuildWorkspaceScope> scopes;
		for (const QString& part : settings.BuildScopes().split(QRegularExpression("[,;\\n]"), Qt::SkipEmptyParts))
		{
			BuildWorkspaceScope scope = BuildWorkspaceScope::Editor;
			const std::string scopeText = part.trimmed().toStdString();
			const bool bScopeParsed = TryParseBuildWorkspaceScope(scopeText, scope);
			const bool bScopeMissing = bScopeParsed && std::find(scopes.begin(), scopes.end(), scope) == scopes.end();

			if (bScopeMissing)
			{
				scopes.push_back(scope);
			}
		}
		return scopes;
	}

	WorkspaceIde ResolveSelectedWorkspaceIde(const LauncherSettings& settings)
	{
		WorkspaceIde ide = WorkspaceIde::VisualStudio;
		TryParseWorkspaceIde(settings.WorkspaceIde().toStdString(), ide);
		return ide;
	}

	QString ResolveSelectedWorkspaceIdeName(const LauncherSettings& settings)
	{
		return QString::fromStdString(DisplayName(ResolveSelectedWorkspaceIde(settings)));
	}

	WorkspaceCompiler ResolveSelectedWorkspaceCompiler(const LauncherSettings& settings)
	{
		WorkspaceCompiler compiler = WorkspaceCompiler::Msvc;
		TryParseWorkspaceCompiler(settings.WorkspaceCompiler().toStdString(), compiler);
		return compiler;
	}

	BuildWorkspaceOperationRequest BuildWorkspacePlanRequest(const std::filesystem::path& workspaceRoot, const LauncherContentModel& contentModel, const LauncherSettings& settings)
	{
		BuildWorkspaceOperationRequest request;
		request.WorkspaceRoot = workspaceRoot;
		request.ContentId = contentModel.ContentId().toStdString();
		request.EditorProfile = settings.EditorProfile().toStdString();
		request.GameProfile = settings.GameProfile().toStdString();
		request.PreferredIde = ResolveSelectedWorkspaceIde(settings);
		request.Compiler = ResolveSelectedWorkspaceCompiler(settings);
		request.SelectedScopes = ResolveSelectedBuildScopes(settings);
		return request;
	}

	LauncherOperationRequest BuildLauncherOperationRequest(
	    const std::filesystem::path& workspaceRoot,
	    const LauncherContentModel& contentModel,
	    const LauncherSettings& settings,
	    const QString& operationId)
	{
		LauncherOperationRequest request;
		request.WorkspaceRoot = workspaceRoot;
		request.OperationId = operationId;
		request.ContentId = contentModel.ContentId();
		request.RunMode = settings.RunMode();
		request.EditorProfile = settings.EditorProfile();
		request.GameProfile = settings.GameProfile();
		request.WorkspaceIde = settings.WorkspaceIde();
		request.WorkspaceCompiler = settings.WorkspaceCompiler();
		request.BuildScopes = settings.BuildScopes();
		request.CookScopes = settings.CookScopes();
		request.ShaderBackend = settings.ShaderBackend();
		request.bShaderSymbols = settings.ShaderSymbols();
		request.GraphicsApi = settings.GraphicsApi();
		request.CaptureProvider = settings.CaptureProvider();
		request.GpuCrashMode = settings.GpuCrashMode();
		request.ShaderExportPath = request.GpuCrashMode == RhiGpuCrashMode::Off ? QString{} : settings.ShaderExportPath();
		request.CleanScope = settings.CleanScope();

		return request;
	}

	static std::vector<std::string> CaptureCleanScopeNames(const QString& selection)
	{
		std::vector<std::string> names;
		for (const QString& name : selection.split(QRegularExpression("[,;\\n]"), Qt::SkipEmptyParts))
		{
			names.push_back(name.toStdString());
		}
		return names;
	}

	ActionCleanTargetRequest BuildActionCleanTargetRequest(
	    const std::filesystem::path& workspaceRoot,
	    const LauncherContentModel& contentModel,
	    const LauncherSettings& settings,
	    const std::filesystem::path& runningLauncherPath,
	    const QString& operationId)
	{
		return ActionCleanTargetRequest{
		    workspaceRoot,
		    runningLauncherPath,
		    operationId.toStdString(),
		    contentModel.ContentId().toStdString(),
		    settings.EditorProfile().toStdString(),
		    settings.GameProfile().toStdString(),
		    CaptureCleanScopeNames(settings.BuildScopes()),
		    CaptureCleanScopeNames(settings.CookScopes())};
	}

	LauncherOperationRequest BuildActionCleanOperationRequest(
	    const std::filesystem::path& workspaceRoot,
	    const LauncherContentModel& contentModel,
	    const LauncherSettings& settings,
	    const std::filesystem::path& runningLauncherPath,
	    const QString& operationId)
	{
		LauncherOperationRequest request = BuildLauncherOperationRequest(workspaceRoot, contentModel, settings, "workspace.clean");
		request.CleanTargets = BuildActionSpecificCleanTargets(BuildActionCleanTargetRequest(workspaceRoot, contentModel, settings, runningLauncherPath, operationId));
		request.bConfirmClean = false;
		return request;
	}

	LauncherOperationRequest BuildScopedCleanOperationRequest(
	    const std::filesystem::path& workspaceRoot,
	    const LauncherContentModel& contentModel,
	    const LauncherSettings& settings,
	    const QString& cleanScope,
	    const std::filesystem::path& runningLauncherPath)
	{
		LauncherOperationRequest request = BuildLauncherOperationRequest(workspaceRoot, contentModel, settings, "workspace.clean");
		request.CleanScope = cleanScope;
		request.CleanTargets.clear();
		request.PreservedPaths.clear();
		const QStringList selectedScopes = cleanScope.split(QRegularExpression("[,;\\n]"), Qt::SkipEmptyParts);
		if ((selectedScopes.contains("artifacts") || selectedScopes.contains("clean-all")) && !runningLauncherPath.empty())
		{
			std::error_code errorCode;
			const std::filesystem::path absoluteRunningPath = std::filesystem::absolute(runningLauncherPath, errorCode);
			const std::filesystem::path candidatePath = errorCode ? runningLauncherPath : absoluteRunningPath;
			const std::filesystem::path launcherDirectory = candidatePath.parent_path();
			if (!launcherDirectory.empty())
			{
				request.PreservedPaths.push_back(QString::fromStdString(launcherDirectory.string()));
			}
		}
		request.bConfirmClean = false;
		return request;
	}

}
