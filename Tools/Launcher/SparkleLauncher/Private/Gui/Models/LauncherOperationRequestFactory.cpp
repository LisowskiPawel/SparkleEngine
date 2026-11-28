#include "Gui/Models/LauncherOperationRequestFactory.h"

#include "Gui/Models/LauncherCleanUiModel.h"
#include "Gui/Models/LauncherContentModel.h"
#include "Gui/App/LauncherSettings.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace SparkleLauncher
{
	template <typename Scope> static std::vector<Scope> ParseSelectedScopes(const QString& selection, bool (*parseScope)(std::string_view, Scope&))
	{
		std::vector<Scope> scopes;
		const QStringList values = selection.split(QRegularExpression("[,;\\n]"), Qt::SkipEmptyParts);
		for (const QString& value : values)
		{
			Scope scope{};
			const std::string text = value.trimmed().toStdString();
			if (!parseScope(text, scope))
			{
				continue;
			}
			if (std::find(scopes.begin(), scopes.end(), scope) == scopes.end())
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
		request.SelectedScopes = ParseSelectedScopes(settings.BuildScopes(), TryParseBuildWorkspaceScope);
		return request;
	}

	LauncherOperationRequest BuildLauncherOperationRequest(
	    const std::filesystem::path& workspaceRoot,
	    const LauncherContentModel& contentModel,
	    const LauncherSettings& settings,
	    const QString& operationId)
	{
		const std::string id = operationId.toStdString();
		const std::string contentId = contentModel.ContentId().toStdString();
		if (FindBuildWorkspaceOperationDefinition(id) != nullptr)
		{
			return {.OperationId = operationId, .Inputs = BuildWorkspacePlanRequest(workspaceRoot, contentModel, settings)};
		}
		if (FindCookOperationDefinition(id) != nullptr)
		{
			CookOperationRequest inputs;
			inputs.WorkspaceRoot = workspaceRoot;
			inputs.ContentId = contentId;
			inputs.GameProfile = settings.GameProfile().toStdString();
			inputs.SelectedScopes = ParseSelectedScopes(settings.CookScopes(), TryParseCookWorkspaceScope);
			inputs.ShaderBackend = settings.ShaderBackend().toStdString();
			inputs.bShaderEnableDebugInfo = settings.ShaderSymbols();
			return {.OperationId = operationId, .Inputs = std::move(inputs)};
		}
		if (FindLevelOperationDefinition(id) != nullptr)
		{
			return {.OperationId = operationId, .Inputs = LevelOperationRequest{.WorkspaceRoot = workspaceRoot, .ContentId = contentId}};
		}
		if (FindLevelRunOperationDefinition(id) != nullptr)
		{
			LevelRunOperationRequest inputs;
			inputs.WorkspaceRoot = workspaceRoot;
			inputs.ContentId = contentId;
			inputs.RunMode = settings.RunMode() == "game" ? LevelRunMode::Game : LevelRunMode::Editor;
			inputs.ProductProfile = inputs.RunMode == LevelRunMode::Game ? settings.GameProfile().toStdString() : settings.EditorProfile().toStdString();
			inputs.GraphicsApi = settings.GraphicsApi().toStdString();
			inputs.CaptureProvider = settings.CaptureProvider();
			inputs.GpuCrashMode = settings.GpuCrashMode();
			if (inputs.GpuCrashMode != RhiGpuCrashMode::Off)
			{
				inputs.ShaderExportPath = settings.ShaderExportPath().toStdWString();
			}
			return {.OperationId = operationId, .Inputs = std::move(inputs)};
		}
		if (FindMaintenanceOperationDefinition(id) != nullptr)
		{
			MaintenanceOperationRequest inputs;
			inputs.WorkspaceRoot = workspaceRoot;
			inputs.ContentId = contentId;
			inputs.EditorProfile = settings.EditorProfile().toStdString();
			return {.OperationId = operationId, .Inputs = std::move(inputs)};
		}

		throw std::invalid_argument("Unknown Launcher operation: " + id);
	}

	ActionCleanTargetRequest BuildActionCleanTargetRequest(
	    const std::filesystem::path& workspaceRoot,
	    const LauncherContentModel& contentModel,
	    const LauncherSettings& settings,
	    const QString& operationId)
	{
		return ActionCleanTargetRequest{
		    workspaceRoot,
		    operationId.toStdString(),
		    contentModel.ContentId().toStdString(),
		    settings.EditorProfile().toStdString(),
		    settings.GameProfile().toStdString(),
		    ParseSelectedScopes(settings.BuildScopes(), TryParseBuildWorkspaceScope),
		    ParseSelectedScopes(settings.CookScopes(), TryParseCookWorkspaceScope)};
	}

	LauncherOperationRequest BuildActionCleanOperationRequest(
	    const std::filesystem::path& workspaceRoot,
	    const LauncherContentModel& contentModel,
	    const LauncherSettings& settings,
	    const QString& operationId)
	{
		LauncherOperationRequest request = BuildLauncherOperationRequest(workspaceRoot, contentModel, settings, "workspace.clean");
		const ActionCleanTargetRequest cleanRequest = BuildActionCleanTargetRequest(workspaceRoot, contentModel, settings, operationId);
		std::get<MaintenanceOperationRequest>(request.Inputs).RequestedCleanTargets = BuildActionCleanPaths(cleanRequest);
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
		const auto scopes = ParseCleanScopeSelection(cleanScope);
		if (!scopes.has_value())
		{
			throw std::invalid_argument("Invalid Launcher clean scope selection.");
		}

		auto& inputs = std::get<MaintenanceOperationRequest>(request.Inputs);
		inputs.RequestedCleanScopes = *scopes;
		const bool bCleansArtifacts = std::find(scopes->begin(), scopes->end(), CleanScope::ArtifactOutputs) != scopes->end();
		const bool bCleansAll = std::find(scopes->begin(), scopes->end(), CleanScope::PristineGeneratedWorkspace) != scopes->end();
		const bool bPreserveLauncher = bCleansArtifacts || bCleansAll;
		if (bPreserveLauncher && !runningLauncherPath.empty())
		{
			std::error_code errorCode;
			const std::filesystem::path absoluteRunningPath = std::filesystem::absolute(runningLauncherPath, errorCode);
			const std::filesystem::path candidatePath = errorCode ? runningLauncherPath : absoluteRunningPath;
			const std::filesystem::path launcherDirectory = candidatePath.parent_path();
			if (!launcherDirectory.empty())
			{
				inputs.PreservedPaths.push_back(launcherDirectory);
			}
		}
		return request;
	}
}
