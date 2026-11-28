#pragma once

#include "Gui/Operations/LauncherOperationRequest.h"
#include "Gui/Operations/LauncherProgressStreamDecoder.h"
#include "SparkleLauncher/OperationModel.h"

#include <QtCore/QHash>
#include <QtCore/QMetaType>
#include <QtCore/QObject>
#include <QtCore/QString>

#include <memory>
#include <string>

namespace SparkleLauncher
{
	class LauncherOperationService;

	class LauncherBackend final : public QObject
	{
		Q_OBJECT

	public:
		explicit LauncherBackend(QObject* parent = nullptr);
		~LauncherBackend() override;

		void RunOperation(LauncherOperationRequest request);
		bool CancelOperation(const QString& runId);

	signals:
		void OperationStarted(const QString& runId, const QString& title);
		void OperationOutputReceived(const QString& runId, const QString& outputText);
		void OperationProgressReceived(const QString& runId, const QString& phase, quint64 completed, quint64 total);
		void OperationFinished(const QString& runId, const OperationRecord& result);

	private:
		void QueueOperationOutput(QString runId, std::string outputText);
		void QueueOperationFinished(QString runId, OperationRecord record);

		QHash<QString, LauncherProgressStreamDecoder> m_progressDecoders;
		std::unique_ptr<LauncherOperationService> m_operationService;
	};
}

Q_DECLARE_METATYPE(SparkleLauncher::OperationRecord)
