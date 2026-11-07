#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Widgets/LauncherActionWidgets.h"
#include "Gui/Style/LauncherUiDesign.h"
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

	QVBoxLayout* AddOptionGroup(QVBoxLayout& layout, const QString& title, const QString& detail)
	{
		QFrame* group = new QFrame(layout.parentWidget());
		group->setObjectName("OptionGroup");
		QVBoxLayout* groupLayout = new QVBoxLayout(group);
		groupLayout->setContentsMargins(LauncherUi::Option::GroupMargins);
		groupLayout->setSpacing(LauncherUi::Option::GroupSpacing);

		QLabel* titleLabel = new QLabel(title, group);
		titleLabel->setObjectName("OptionGroupTitle");
		groupLayout->addWidget(titleLabel);

		if (!detail.isEmpty())
		{
			QLabel* detailLabel = new QLabel(detail, group);
			detailLabel->setObjectName("OptionHelpText");
			detailLabel->setWordWrap(true);
			groupLayout->addWidget(detailLabel);
		}

		layout.addWidget(group);
		return groupLayout;
	}

	void AddNoOptionsMessage(QVBoxLayout& layout, const QString& text)
	{
		QLabel* label = new QLabel(text, layout.parentWidget());
		label->setObjectName("MutedLabel");
		label->setAccessibleName(text);
		label->setWordWrap(true);
		layout.addWidget(label);
	}

	QLabel* AddStatusRow(QVBoxLayout& layout, const QString& label, const QString& status, const QString& detail, const QString& state, QWidget* accessory)
	{
		QFrame* row = new QFrame(layout.parentWidget());
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

	QLabel* CreateSectionLabel(const QString& title)
	{
		QLabel* label = new QLabel(title);
		label->setObjectName("SectionLabel");
		label->setAccessibleName(title);
		return label;
	}

	QLabel* CreateFieldLabel(const QString& title)
	{
		QLabel* label = new QLabel(title);
		label->setObjectName("FieldLabel");
		label->setAccessibleName(title);
		return label;
	}
}
