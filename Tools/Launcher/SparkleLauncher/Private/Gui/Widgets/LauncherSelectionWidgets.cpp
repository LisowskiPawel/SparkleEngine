#include "Gui/Style/LauncherUiDesign.h"
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QStyle>
#include "Gui/Widgets/LauncherSelectionWidgets.h"

#include <QtCore/QObject>
#include <QtCore/QSignalBlocker>
#include <QtCore/Qt>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>

#include <algorithm>

namespace SparkleLauncher
{
	static void SetScopeBoxChecked(QCheckBox* scopeBox, bool bChecked)
	{
		if (scopeBox == nullptr || !scopeBox->isEnabled())
		{
			return;
		}

		const QSignalBlocker blocker(scopeBox);
		scopeBox->setChecked(bChecked);
		if (QWidget* scopeRow = scopeBox->parentWidget())
		{
			scopeRow->setProperty("Selected", bChecked);
			scopeRow->style()->unpolish(scopeRow);
			scopeRow->style()->polish(scopeRow);
		}
	}

	static void SetAllScopeBoxesChecked(const QVector<QCheckBox*>& scopeBoxes, bool bChecked)
	{
		for (QCheckBox* scopeBox : scopeBoxes)
		{
			SetScopeBoxChecked(scopeBox, bChecked);
		}
	}

	static bool AreAllAvailableScopesChecked(const QVector<QCheckBox*>& scopeBoxes)
	{
		bool bHasAvailableScope = false;
		for (const QCheckBox* scopeBox : scopeBoxes)
		{
			if (scopeBox == nullptr || !scopeBox->isEnabled())
			{
				continue;
			}

			bHasAvailableScope = true;
			if (!scopeBox->isChecked())
			{
				return false;
			}
		}
		return bHasAvailableScope;
	}

	static void UpdateSelectAllBox(QCheckBox* selectAllBox, const QVector<QCheckBox*>& scopeBoxes)
	{
		SetScopeBoxChecked(selectAllBox, AreAllAvailableScopesChecked(scopeBoxes));
	}

	void ConnectSelectAllScopeBox(QCheckBox* selectAllBox, const QVector<QCheckBox*>& scopeBoxes, QObject* context, std::function<void(bool)> commitSelection)
	{
		for (QCheckBox* scopeBox : scopeBoxes)
		{
			QObject::connect(
			    scopeBox,
			    &QCheckBox::toggled,
			    context,
			    [scopeBoxes, selectAllBox, commitSelection]()
			    {
				    commitSelection(false);
				    UpdateSelectAllBox(selectAllBox, scopeBoxes);
			    });
		}

		QObject::connect(
		    selectAllBox,
		    &QCheckBox::toggled,
		    context,
		    [scopeBoxes, selectAllBox, commitSelection](bool bSelected)
		    {
			    SetAllScopeBoxesChecked(scopeBoxes, bSelected);
			    commitSelection(bSelected);
			    UpdateSelectAllBox(selectAllBox, scopeBoxes);
		    });

		UpdateSelectAllBox(selectAllBox, scopeBoxes);
	}

	QStringList CollectSelectedScopeValues(const QVector<QCheckBox*>& scopeBoxes)
	{
		QStringList selectedValues;
		for (const QCheckBox* scopeBox : scopeBoxes)
		{
			if (scopeBox != nullptr && scopeBox->isEnabled() && scopeBox->isChecked())
			{
				selectedValues.push_back(scopeBox->property("ScopeValue").toString());
			}
		}
		return selectedValues;
	}

	static QStandardItem* ComboItem(QComboBox& combo, int row)
	{
		QStandardItemModel* model = qobject_cast<QStandardItemModel*>(combo.model());
		return model == nullptr ? nullptr : model->item(row);
	}

	static void AppendGroupHeading(QComboBox& combo, const QString& label)
	{
		combo.addItem(label);
		const int row = combo.count() - 1;
		combo.setItemData(row, label + " options", Qt::AccessibleTextRole);
		if (QStandardItem* item = ComboItem(combo, row))
		{
			QFont font = item->font();
			font.setBold(true);
			item->setFont(font);
			item->setForeground(QBrush(QColor(LauncherUi::Color::TextSecondary)));
			item->setFlags(Qt::NoItemFlags);
		}
	}

	static int AppendOption(QComboBox& combo, const LauncherSelectionOption& option)
	{
		combo.addItem(option.DisplayName, option.Value);
		const int row = combo.count() - 1;

		const QString availability = option.bAvailable ? QStringLiteral("Available")
		                                              : (option.bSupported ? QStringLiteral("Supported, setup required") : QStringLiteral("Unavailable in this configuration"));

		const QString detail = option.Detail.isEmpty() ? availability : availability + ". " + option.Detail;
		combo.setItemData(row, detail, Qt::ToolTipRole);
		combo.setItemData(row, option.DisplayName, Qt::AccessibleTextRole);
		combo.setItemData(row, detail, Qt::AccessibleDescriptionRole);
		if (!option.bAvailable)
		{
			if (QStandardItem* item = ComboItem(combo, row))
			{
				item->setEnabled(false);
				item->setForeground(QBrush(QColor(LauncherUi::Color::TextMuted)));
			}
		}
		return row;
	}

	QString PopulateLauncherSelectionCombo(QComboBox& combo, const QVector<LauncherSelectionOption>& options, const QString& preferredValue)
	{
		const QSignalBlocker blocker(&combo);
		combo.clear();
		combo.setPlaceholderText("None available");

		int selectedIndex = -1;
		int firstAvailableIndex = -1;

		const auto appendOptions = [&](bool bAvailable, bool bSupported)
		{
			for (const LauncherSelectionOption& option : options)
			{
				if (option.bAvailable != bAvailable || (!bAvailable && option.bSupported != bSupported))
				{
					continue;
				}

				const int row = AppendOption(combo, option);
				if (bAvailable && firstAvailableIndex < 0)
				{
					firstAvailableIndex = row;
				}
				if (bAvailable && option.Value == preferredValue)
				{
					selectedIndex = row;
				}
			}
		};

		AppendGroupHeading(combo, "Available");
		appendOptions(true, true);
		const bool bHasSupportedOptions = std::any_of(options.begin(), options.end(), [](const LauncherSelectionOption& option) { return !option.bAvailable && option.bSupported; });
		if (bHasSupportedOptions)
		{
			combo.insertSeparator(combo.count());
			AppendGroupHeading(combo, "Supported");
			appendOptions(false, true);
		}

		const bool bHasUnsupportedOptions = std::any_of(options.begin(), options.end(), [](const LauncherSelectionOption& option) { return !option.bAvailable && !option.bSupported; });
		if (bHasUnsupportedOptions)
		{
			combo.insertSeparator(combo.count());
			AppendGroupHeading(combo, "Unavailable for selection");
			appendOptions(false, false);
		}

		selectedIndex = selectedIndex >= 0 ? selectedIndex : firstAvailableIndex;
		combo.setCurrentIndex(selectedIndex);
		combo.setEnabled(!options.empty());
		const int availableCount = static_cast<int>(std::count_if(options.begin(), options.end(), [](const LauncherSelectionOption& option) { return option.bAvailable; }));
		combo.setAccessibleDescription(QStringLiteral("%1 available; %2 known but unavailable.").arg(availableCount).arg(options.size() - availableCount));
		const int contentWidth = combo.view()->sizeHintForColumn(0) + LauncherUi::Selector::PopupHorizontalPadding;
		combo.view()->setMinimumWidth(qBound(combo.minimumWidth(), contentWidth, LauncherUi::Selector::PopupMaxWidth));
		return selectedIndex < 0 ? QString() : combo.currentData().toString();
	}

	void AddWorkflowScopeRow(
	    QVBoxLayout& layout,
	    const QString& label,
	    const QString& value,
	    const QString& detail,
	    const QString& metadata,
	    bool bAvailable,
	    const QStringList& selectedScopes,
	    QVector<QCheckBox*>& scopeBoxes,
	    const std::function<void(QWidget*)>& registerFocusable)
	{
		QFrame* scopeRow = new QFrame(layout.parentWidget());
		scopeRow->setObjectName("WorkflowScopeRow");
		QHBoxLayout* scopeRowLayout = new QHBoxLayout(scopeRow);
		scopeRowLayout->setContentsMargins(LauncherUi::ScopeSelection::RowMargins);
		scopeRowLayout->setSpacing(LauncherUi::ScopeSelection::RowSpacing);

		QVBoxLayout* descriptionLayout = new QVBoxLayout();
		descriptionLayout->setContentsMargins(0, 0, 0, 0);
		descriptionLayout->setSpacing(2);
		QCheckBox* scopeBox = new QCheckBox(label, scopeRow);
		scopeBox->setObjectName("WorkflowScopeCheckBox");
		scopeBox->setProperty("ScopeValue", value);
		scopeBox->setToolTip(detail);
		scopeBox->setChecked(bAvailable && selectedScopes.contains(value));
		scopeBox->setEnabled(bAvailable);
		registerFocusable(scopeBox);
		descriptionLayout->addWidget(scopeBox);

		QLabel* scopeDescription = new QLabel(detail, scopeRow);
		scopeDescription->setObjectName("WorkflowScopeDescription");
		scopeDescription->setWordWrap(true);
		descriptionLayout->addWidget(scopeDescription);
		scopeRowLayout->addLayout(descriptionLayout, 1);

		QLabel* scopeMetadata = new QLabel(metadata, scopeRow);
		scopeMetadata->setObjectName("WorkflowScopeMetadata");
		scopeMetadata->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		scopeRowLayout->addWidget(scopeMetadata, 0);

		scopeRow->setProperty("Selected", scopeBox->isChecked());

		QObject::connect(
		    scopeBox,
		    &QCheckBox::toggled,
		    scopeRow,
		    [scopeRow](bool bSelected)
		    {
			    scopeRow->setProperty("Selected", bSelected);
			    scopeRow->style()->unpolish(scopeRow);
			    scopeRow->style()->polish(scopeRow);
		    });

		layout.addWidget(scopeRow);
		scopeBoxes.push_back(scopeBox);
	}

	void AddWorkflowAutomationNote(QVBoxLayout& layout, const QString& detail)
	{
		QFrame* automationNote = new QFrame(layout.parentWidget());
		automationNote->setObjectName("WorkflowAutomationNote");
		automationNote->setMinimumWidth(LauncherUi::ScopeSelection::ContentMinWidth);
		automationNote->setMaximumWidth(LauncherUi::ScopeSelection::ContentMaxWidth);
		QHBoxLayout* automationLayout = new QHBoxLayout(automationNote);
		automationLayout->setContentsMargins(LauncherUi::ScopeSelection::RowMargins);
		automationLayout->setSpacing(LauncherUi::ScopeSelection::RowSpacing);
		QLabel* automationTitle = new QLabel("AUTOMATIC", automationNote);
		automationTitle->setObjectName("WorkflowAutomationTitle");
		automationLayout->addWidget(automationTitle, 0, Qt::AlignTop);
		QLabel* automationDetail = new QLabel(detail, automationNote);
		automationDetail->setObjectName("WorkflowAutomationDetail");
		automationDetail->setWordWrap(true);
		automationLayout->addWidget(automationDetail, 1);
		layout.addWidget(automationNote);
	}
}
