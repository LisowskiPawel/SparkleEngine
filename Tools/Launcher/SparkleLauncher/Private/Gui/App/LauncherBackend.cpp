#include "LauncherBackend.h"

#include "Operations/LauncherOperationService.h"

#include <QtCore/QMetaObject>

#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace SparkleLauncher
{

	static LauncherOperationResult BuildOperationResult(const OperationRecord& operation)
	{
		LauncherOperationResult result;
		result.GpuCrash = operation.GpuCrash;
		result.ShaderExportPath = QString::fromStdWString(operation.ShaderExportPath.wstring());
		result.Status = QString::fromStdString(ToString(operation.Status));
		result.Succeeded = operation.Status == OperationStatus::Succeeded;
		result.Skipped = operation.Status == OperationStatus::Skipped;
		result.Canceled = operation.Status == OperationStatus::Canceled;
		if (operation.ExitCode.has_value())
		{
			result.ExitCode = *operation.ExitCode;
			result.HasExitCode = true;
		}
		if (!operation.LogPath.empty())
		{
			result.LogPath = QString::fromStdString(operation.LogPath.string());
		}
		if (operation.Failure.has_value())
		{
			result.ProblemKind = QString::fromStdString(ToString(operation.Failure->Kind));
			result.Problem = QString::fromStdString(operation.Failure->Summary);
			result.ExpectedAction = QString::fromStdString(operation.Failure->ExpectedAction);
		}
		return result;
	}

	LauncherBackend::LauncherBackend(QObject* parent) :
	    LauncherBackend({}, parent)
	{
	}

	LauncherBackend::LauncherBackend(ProcessRunnerFactory processRunnerFactory, QObject* parent) :
	    QObject(parent)
	{
		qRegisterMetaType<LauncherOperationResult>();
		if (!processRunnerFactory)
		{
			processRunnerFactory = []()
			{
				return std::make_unique<NativeProcessRunner>();
			};
		}
		m_operationService = std::make_unique<LauncherOperationService>(std::move(processRunnerFactory));
		PopulateOperationCatalog();
	}

	LauncherBackend::~LauncherBackend() = default;

	const QVector<LauncherOperationDescriptor>& LauncherBackend::Operations() const
	{
		return m_operations;
	}

	void LauncherBackend::RunOperation(LauncherOperationRequest request)
	{
		const QString runId = request.RunId.isEmpty() ? request.OperationId : request.RunId;
		const LauncherOperationDescriptor* operation = FindOperation(request.OperationId);
		if (operation == nullptr)
		{
			OperationRecord rejected = MakeOperationRecord(request.OperationId.toStdString(), request.OperationId.toStdString());
			SetOperationFailure(rejected, OperationProblemKind::Planning, "Unknown launcher operation.", "Select an available workflow, refresh the launcher, then retry.");
			MarkOperationFinished(rejected, OperationStatus::Failed);
			emit OperationFinished(runId, request.OperationId, request.OperationId, BuildOperationResult(rejected));
			return;
		}

		emit OperationStarted(runId, operation->Id, operation->DisplayName);
		const LauncherOperationCategory category = operation->Category;
		const QString title = operation->DisplayName;
		const QString operationIdText = operation->Id;

		m_operationService->Launch(
		    category,
		    std::move(request),
		    title.toStdString(),
		    [this, runId, operationIdText](std::string_view output) { QueueOperationOutput(runId, operationIdText, std::string(output)); },
		    [this, runId, operationIdText, title](const OperationRecord& record) { QueueOperationFinished(runId, operationIdText, title, record); });
	}

	bool LauncherBackend::ObserveGpuCrash(GpuCrashIncident incident, std::function<void(const QString&)> completion)
	{
		return m_operationService->ObserveGpuCrash(
		    std::move(incident),
		    [this, completion = std::move(completion)](std::string state)
		    { QMetaObject::invokeMethod(this, [completion, state = QString::fromStdString(state)]() { completion(state); }, Qt::QueuedConnection); });
	}

	bool LauncherBackend::CancelOperation(const QString& runId)
	{
		return !runId.isEmpty() && m_operationService->Cancel(runId.toStdString());
	}

	void LauncherBackend::QueueOperationOutput(QString runId, QString operationId, std::string outputText)
	{
		QMetaObject::invokeMethod(
		    this,
		    [this, runId = std::move(runId), operationId = std::move(operationId), outputText = std::move(outputText)]
		    {
			    m_progressDecoders[runId].Consume(
			        outputText,
			        [this, &runId, &operationId](std::string_view output)
			        { emit OperationOutputReceived(runId, operationId, QString::fromUtf8(output.data(), static_cast<qsizetype>(output.size()))); },
			        [this, &runId, &operationId](const ToolWorkProgress& progress)
			        {
				        emit OperationProgressReceived(
				            runId,
				            operationId,
				            QString::fromUtf8(progress.phase.data(), static_cast<qsizetype>(progress.phase.size())),
				            static_cast<quint64>(progress.completed),
				            static_cast<quint64>(progress.total));
			        });
		    },
		    Qt::QueuedConnection);
	}

	void LauncherBackend::QueueOperationFinished(QString runId, QString operationId, QString title, const OperationRecord& record)
	{
		const LauncherOperationResult result = BuildOperationResult(record);

		QMetaObject::invokeMethod(
		    this,
		    [this, runId = std::move(runId), operationId = std::move(operationId), title = std::move(title), result]
		    {
			    auto progressDecoder = m_progressDecoders.find(runId);
			    if (progressDecoder != m_progressDecoders.end())
			    {
				    progressDecoder->Flush([this, &runId, &operationId](std::string_view output)
				        { emit OperationOutputReceived(runId, operationId, QString::fromUtf8(output.data(), static_cast<qsizetype>(output.size()))); });
				    m_progressDecoders.erase(progressDecoder);
			    }
			    emit OperationFinished(runId, operationId, title, result);
		    },
		    Qt::QueuedConnection);
	}

	void LauncherBackend::PopulateOperationCatalog()
	{
		m_operations.clear();

		for (const BuildWorkspaceOperationDefinition& definition : GetBuildWorkspaceOperationDefinitions())
		{
			m_operations.push_back(
			    {QString::fromStdString(definition.Id),
			        QString::fromStdString(definition.Group),
			        QString::fromStdString(definition.DisplayName),
			        QString::fromStdString(definition.Description),
			        LauncherOperationCategory::Workspace});
		}

		for (const CookOperationDefinition& definition : GetCookOperationDefinitions())
		{
			m_operations.push_back(
			    {QString::fromStdString(definition.Id),
			        QString::fromStdString(definition.Group),
			        QString::fromStdString(definition.DisplayName),
			        QString::fromStdString(definition.Description),
			        LauncherOperationCategory::Cooking});
		}

		for (const LevelOperationDefinition& definition : GetLevelOperationDefinitions())
		{
			m_operations.push_back(
			    {QString::fromStdString(definition.Id),
			        QString::fromStdString(definition.Group),
			        QString::fromStdString(definition.DisplayName),
			        QString::fromStdString(definition.Description),
			        LauncherOperationCategory::Levels});
		}

		for (const LevelRunOperationDefinition& definition : GetLevelRunOperationDefinitions())
		{
			m_operations.push_back(
			    {QString::fromStdString(definition.Id),
			        QString::fromStdString(definition.Group),
			        QString::fromStdString(definition.DisplayName),
			        QString::fromStdString(definition.Description),
			        LauncherOperationCategory::LevelRun});
		}

		for (const MaintenanceOperationDefinition& definition : GetMaintenanceOperationDefinitions())
		{
			m_operations.push_back(
			    {QString::fromStdString(definition.Id),
			        QString::fromStdString(definition.Group),
			        QString::fromStdString(definition.DisplayName),
			        QString::fromStdString(definition.Description),
			        LauncherOperationCategory::Maintenance});
		}
	}

	const LauncherOperationDescriptor* LauncherBackend::FindOperation(const QString& operationId) const
	{
		for (const LauncherOperationDescriptor& operation : m_operations)
		{
			if (operation.Id == operationId)
			{
				return &operation;
			}
		}

		return nullptr;
	}
}
