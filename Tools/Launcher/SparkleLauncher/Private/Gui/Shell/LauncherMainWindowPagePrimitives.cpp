#include "LauncherMainWindow.h"

#include "LauncherActionWidgets.h"
#include "LauncherUiDesign.h"

#include <QtCore/QString>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <algorithm>

namespace SparkleLauncher
{
	static constexpr int kSpaceSmall = LauncherUi::Space::Small;
	static constexpr int kSpaceMedium = LauncherUi::Space::Medium;

	QLabel* LauncherMainWindow::AddStatusRow(QVBoxLayout& layout, const QString& label, const QString& status, const QString& detail, const QString& state, QWidget* accessory)
	{
		QFrame* row = new QFrame(this);
		row->setObjectName("StatusRow");
		QHBoxLayout* rowLayout = new QHBoxLayout(row);
		rowLayout->setContentsMargins(0, 0, 0, 0);
		rowLayout->setSpacing(kSpaceMedium);

		QVBoxLayout* textLayout = new QVBoxLayout();
		textLayout->setContentsMargins(0, 0, 0, 0);
		textLayout->setSpacing(LauncherUi::Option::StatusDetailSpacing);

		QLabel* nameLabel = new QLabel(label, row);
		nameLabel->setObjectName("StatusLabel");
		textLayout->addWidget(nameLabel);

		QHBoxLayout* metadataLayout = new QHBoxLayout();
		metadataLayout->setContentsMargins(0, 0, 0, 0);
		metadataLayout->setSpacing(kSpaceSmall);

		QLabel* statusLabel = new QLabel(row);
		ApplyInlineStatusLabel(*statusLabel, status, state);
		statusLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		metadataLayout->addWidget(statusLabel, 0, Qt::AlignVCenter);

		if (!detail.isEmpty())
		{
			QLabel* detailLabel = new QLabel(detail, row);
			detailLabel->setObjectName("StatusDetail");
			detailLabel->setWordWrap(true);
			metadataLayout->addWidget(detailLabel, 1, Qt::AlignVCenter);
		}
		textLayout->addLayout(metadataLayout);

		rowLayout->addLayout(textLayout, 1);

		if (accessory != nullptr)
		{
			QWidget* actionCell = new QWidget(row);
			actionCell->setObjectName("StatusActionCell");
			const int actionWidth = std::max(LauncherUi::Row::StatusActionWidth, accessory->sizeHint().width());
			actionCell->setFixedWidth(actionWidth);
			QHBoxLayout* actionCellLayout = new QHBoxLayout(actionCell);
			actionCellLayout->setContentsMargins(0, 0, 0, 0);
			actionCellLayout->setSpacing(0);
			actionCellLayout->setAlignment(Qt::AlignCenter);
			accessory->setParent(actionCell);
			actionCellLayout->addWidget(accessory, 0, Qt::AlignCenter);
			rowLayout->addWidget(actionCell, 0, Qt::AlignRight | Qt::AlignVCenter);
		}

		layout.addWidget(row);
		return statusLabel;
	}

}
