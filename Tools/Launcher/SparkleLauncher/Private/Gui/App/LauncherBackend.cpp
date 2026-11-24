#include "Gui/App/LauncherBackend.h"

#include "Gui/Operations/LauncherOperationService.h"
#include "Operations/LauncherOperationExecution.h"

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
		result.Status = operation.Status;
		result.ExitCode = operation.ExitCode;
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
	    QObject(parent)
	{
		qRegisterMetaType<LauncherOperationResult>();
		m_operationService = std::make_unique<LauncherOperationService>();
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
		const QString title = operation->DisplayName;
		const QString operationIdText = operation->Id;

		m_operationService->Launch(
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
		    {
			    const auto stateText = QString::fromStdString(state);
			    auto notifyCompletion = [completion, stateText]()
			    {
				    completion(stateText);
			    };
			    QMetaObject::invokeMethod(this, std::move(notifyCompletion), Qt::QueuedConnection);
		    });
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
			        {
				        const auto text = QString::fromUtf8(output.data(), static_cast<qsizetype>(output.size()));
				        emit OperationOutputReceived(runId, operationId, text);
			        },
			        [this, &runId, &operationId](const ToolWorkProgress& progress)
			        {
				        const auto phase = QString::fromUtf8(progress.phase.data(), static_cast<qsizetype>(progress.phase.size()));

				        emit OperationProgressReceived(runId, operationId, phase, static_cast<quint64>(progress.completed), static_cast<quint64>(progress.total));
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
				    progressDecoder->Flush(
				        [this, &runId, &operationId](std::string_view output)
				        {
					        const auto text = QString::fromUtf8(output.data(), static_cast<qsizetype>(output.size()));
					        emit OperationOutputReceived(runId, operationId, text);
				        });
				    m_progressDecoders.erase(progressDecoder);
			    }
			    emit OperationFinished(runId, operationId, title, result);
		    },
		    Qt::QueuedConnection);
	}

	void LauncherBackend::PopulateOperationCatalog()
	{
		m_operations.clear();

		VisitLauncherOperationDefinitions(
		    [this](const auto& definition)
		    {
			    m_operations.push_back(
			        {QString::fromStdString(definition.Id), QString::fromStdString(definition.Group), QString::fromStdString(definition.DisplayName), QString::fromStdString(definition.Description)});
		    });
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
