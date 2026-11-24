#pragma once

#include "Gui/Operations/LauncherOperationRequest.h"
#include "Gui/Operations/LauncherProgressStreamDecoder.h"
#include "Gui/Models/LauncherOperationResult.h"
#include "SparkleLauncher/OperationModel.h"
#include "SparkleLauncher/LauncherProcess.h"

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>

#include <functional>
#include <memory>
#include <string>

namespace SparkleLauncher
{
	class LauncherOperationService;
	struct OperationRecord;

	struct LauncherOperationDescriptor
	{
		QString Id;
		QString Group;
		QString DisplayName;
		QString Description;
	};

	class LauncherBackend final : public QObject
	{
		Q_OBJECT

	public:
		explicit LauncherBackend(QObject* parent = nullptr);
		~LauncherBackend() override;

		const QVector<LauncherOperationDescriptor>& Operations() const;

		void RunOperation(LauncherOperationRequest request);
		bool CancelOperation(const QString& runId);
		bool ObserveGpuCrash(GpuCrashIncident incident, std::function<void(const QString&)> completion);

	signals:
		void OperationStarted(const QString& runId, const QString& operationId, const QString& title);
		void OperationOutputReceived(const QString& runId, const QString& operationId, const QString& outputText);
		void OperationProgressReceived(const QString& runId, const QString& operationId, const QString& phase, quint64 completed, quint64 total);
		void OperationFinished(const QString& runId, const QString& operationId, const QString& title, const LauncherOperationResult& result);

	private:
		void PopulateOperationCatalog();
		void QueueOperationOutput(QString runId, QString operationId, std::string outputText);
		void QueueOperationFinished(QString runId, QString operationId, QString title, const OperationRecord& record);
		const LauncherOperationDescriptor* FindOperation(const QString& operationId) const;

		QVector<LauncherOperationDescriptor> m_operations;
		QHash<QString, LauncherProgressStreamDecoder> m_progressDecoders;
		std::unique_ptr<LauncherOperationService> m_operationService;
	};
}
