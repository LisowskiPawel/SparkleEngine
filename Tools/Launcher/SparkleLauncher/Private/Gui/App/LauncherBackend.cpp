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
	LauncherBackend::LauncherBackend(QObject* parent) :
	    QObject(parent)
	{
		qRegisterMetaType<OperationRecord>();
		m_operationService = std::make_unique<LauncherOperationService>();
	}

	LauncherBackend::~LauncherBackend() = default;

	void LauncherBackend::RunOperation(LauncherOperationRequest request)
	{
		const QString runId = request.RunId.isEmpty() ? request.OperationId : request.RunId;
		const auto displayName = FindLauncherOperationDisplayName(request.OperationId.toStdString());
		if (!displayName)
		{
			OperationRecord rejected = MakeOperationRecord(request.OperationId.toStdString(), request.OperationId.toStdString());
			SetOperationFailure(rejected, OperationProblemKind::Planning, "Unknown launcher operation.", "Select an available workflow, refresh the launcher, then retry.");
			MarkOperationFinished(rejected, OperationStatus::Failed);
			emit OperationFinished(runId, rejected);
			return;
		}

		const QString title = QString::fromUtf8(displayName->data(), static_cast<qsizetype>(displayName->size()));
		emit OperationStarted(runId, title);

		m_operationService->Launch(
		    std::move(request),
		    title.toStdString(),
		    [this, runId](std::string_view output) { QueueOperationOutput(runId, std::string(output)); },
		    [this, runId](OperationRecord record) { QueueOperationFinished(runId, std::move(record)); });
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

	void LauncherBackend::QueueOperationOutput(QString runId, std::string outputText)
	{
		QMetaObject::invokeMethod(
		    this,
		    [this, runId = std::move(runId), outputText = std::move(outputText)]
		    {
			    m_progressDecoders[runId].Consume(
			        outputText,
			        [this, &runId](std::string_view output)
			        {
				        const auto text = QString::fromUtf8(output.data(), static_cast<qsizetype>(output.size()));
				        emit OperationOutputReceived(runId, text);
			        },
			        [this, &runId](const ToolWorkProgress& progress)
			        {
				        const auto phase = QString::fromUtf8(progress.phase.data(), static_cast<qsizetype>(progress.phase.size()));

				        emit OperationProgressReceived(runId, phase, static_cast<quint64>(progress.completed), static_cast<quint64>(progress.total));
			        });
		    },
		    Qt::QueuedConnection);
	}

	void LauncherBackend::QueueOperationFinished(QString runId, OperationRecord record)
	{
		QMetaObject::invokeMethod(
		    this,
		    [this, runId = std::move(runId), record = std::move(record)]
		    {
			    auto progressDecoder = m_progressDecoders.find(runId);
			    if (progressDecoder != m_progressDecoders.end())
			    {
				    progressDecoder->Flush(
				        [this, &runId](std::string_view output)
				        {
					        const auto text = QString::fromUtf8(output.data(), static_cast<qsizetype>(output.size()));
					        emit OperationOutputReceived(runId, text);
				        });
				    m_progressDecoders.erase(progressDecoder);
			    }
			    emit OperationFinished(runId, record);
		    },
		    Qt::QueuedConnection);
	}
}
