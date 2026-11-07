#include "Gui/Pages/Clean/LauncherCleanOptions.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Widgets/LauncherSelectionWidgets.h"
#include "Gui/Widgets/LauncherLayoutWidgets.h"
#include "Gui/Models/LauncherCleanUiModel.h"
#include "Gui/App/LauncherSettings.h"
#include "Gui/Style/LauncherUiDesign.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QStyle>

#include <array>
#include <system_error>

namespace SparkleLauncher
{
	static QString ToDisplayPath(const std::filesystem::path& workspaceRoot, const std::filesystem::path& path)
	{
		std::error_code errorCode;
		const std::filesystem::path relative = std::filesystem::relative(path, workspaceRoot, errorCode);
		return QString::fromStdString((!errorCode && !relative.empty()) ? relative.generic_string() : path.generic_string());
	}

	static QString FormatDirectoryInventory(const std::filesystem::path& path)
	{
		std::error_code errorCode;
		if (!std::filesystem::exists(path, errorCode) || errorCode)
		{
			return "not present";
		}
		if (std::filesystem::is_regular_file(path, errorCode))
		{
			return "1 file";
		}

		std::uintmax_t fileCount = 0;
		std::uintmax_t directoryCount = 0;
		if (std::filesystem::is_directory(path, errorCode))
		{
			std::filesystem::recursive_directory_iterator iterator(path, std::filesystem::directory_options::skip_permission_denied, errorCode);
			const std::filesystem::recursive_directory_iterator end;
			while (iterator != end)
			{
				const std::filesystem::directory_entry entry = *iterator;
				if (entry.is_directory(errorCode))
				{
					++directoryCount;
				}
				else if (entry.is_regular_file(errorCode))
				{
					++fileCount;
				}
				errorCode.clear();
				iterator.increment(errorCode);
				errorCode.clear();
			}
		}

		return QStringLiteral("%1 files, %2 folders").arg(fileCount).arg(directoryCount);
	}

	static void AddCleanScopeRow(
	    QVBoxLayout& layout,
	    const CleanScopeUiOption& scope,
	    const QString& contentId,
	    const QStringList& selectedScopes,
	    QVector<QCheckBox*>& scopeBoxes,
	    const std::filesystem::path& workspaceRoot,
	    const std::function<void(QWidget*)>& registerFocusable)
	{
		QFrame* scopeRow = new QFrame(layout.parentWidget());
		scopeRow->setObjectName("CleanScopeRow");
		QHBoxLayout* scopeRowLayout = new QHBoxLayout(scopeRow);
		scopeRowLayout->setContentsMargins(LauncherUi::Clean::RowMargins);
		scopeRowLayout->setSpacing(LauncherUi::Clean::RowSpacing);

		QVBoxLayout* descriptionLayout = new QVBoxLayout();
		descriptionLayout->setContentsMargins(0, 0, 0, 0);
		descriptionLayout->setSpacing(2);

		QCheckBox* scopeBox = new QCheckBox(scope.Label, scopeRow);
		scopeBox->setObjectName("CleanScopeCheckBox");
		scopeBox->setToolTip(scope.Detail);
		scopeBox->setProperty("ScopeValue", scope.Value);
		scopeBox->setChecked(selectedScopes.contains("clean-all") || selectedScopes.contains(scope.Value));
		registerFocusable(scopeBox);
		descriptionLayout->addWidget(scopeBox);

		QLabel* scopeDescription = new QLabel(scope.Detail, scopeRow);
		scopeDescription->setObjectName("CleanScopeDescription");
		scopeDescription->setWordWrap(true);
		descriptionLayout->addWidget(scopeDescription);
		scopeRowLayout->addLayout(descriptionLayout, 3);

		const std::filesystem::path previewPath = ResolveCleanScopePreviewPath(workspaceRoot, contentId, scope.Value);

		const QString previewText = scope.Value == "cooked" ? QStringLiteral("Generated content - ") + FormatDirectoryInventory(previewPath)
		                                                    : (scope.Preview.isEmpty() ? ToDisplayPath(workspaceRoot, previewPath) + " - " + FormatDirectoryInventory(previewPath) : scope.Preview);

		ElidedLabel* scopeDetail = new ElidedLabel(previewText, scopeRow);
		scopeDetail->setObjectName("CleanScopePreview");
		scopeDetail->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		scopeDetail->setMaximumWidth(380);
		scopeRowLayout->addWidget(scopeDetail, 2);

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

	void BuildLauncherCleanOptions(
	    QVBoxLayout& layout,
	    const std::filesystem::path& workspaceRoot,
	    const QString& contentId,
	    LauncherSettings& settings,
	    const std::function<void(QWidget*)>& registerFocusable,
	    const std::function<void()>& selectionChanged)
	{
		const std::array<CleanScopeUiOption, 6> cleanScopes{{
		    {"Cooked content", "cooked", "Remove generated cooked assets for this workspace.", QString(), "Content outputs"},
		    {"Build workspace",
		        "build-tree",
		        "Remove build intermediates and generated CMake or IDE files; keep downloaded dependencies.",
		        "build content except build/_deps and generated workspace files",
		        "Build outputs"},
		    {"Generated artifacts", "artifacts", "Remove executables, libraries, symbols, diagnostics, and all cooked content.", QString(), "Build outputs"},
		    {"IDE and workspace state",
		        "workspace-state",
		        "Reset local Visual Studio, Rider, VS Code, and ImGui workspace state.",
		        ".vs, .vscode, .idea, and ImGui workspace state",
		        "Local state and caches"},
		    {"Source dependency cache", "deps", "Remove downloaded code dependencies; the next sync or configure downloads them again.", QString(), "Local state and caches"},
		    {"Logs",
		        "logs",
		        "Remove product and launcher logs from per-user state plus any legacy repository/content logs; keep the current launcher "
		        "log until exit.",
		        "product, launcher, and legacy logs",
		        "Local state and caches"},
		}};

		const std::array<QString, 3> cleanGroups{{
		    "Content outputs",
		    "Build outputs",
		    "Local state and caches",
		}};

		QVector<QCheckBox*> scopeBoxes;
		QStringList selectedScopes = settings.CleanScope().split(QRegularExpression("[,;\\n]"), Qt::SkipEmptyParts);

		QVBoxLayout* cleanLayout = AddOptionGroup(
		    layout,
		    "Choose generated data",
		    "Select only what should be regenerated. Project source files and synced levels are preserved. You will confirm before "
		    "cleaning.");

		QFrame* selectionPanel = new QFrame(cleanLayout->parentWidget());
		selectionPanel->setObjectName("CleanSelectionPanel");
		selectionPanel->setMinimumWidth(LauncherUi::ScopeSelection::ContentMinWidth);
		selectionPanel->setMaximumWidth(LauncherUi::ScopeSelection::ContentMaxWidth);
		QVBoxLayout* selectionLayout = new QVBoxLayout(selectionPanel);
		selectionLayout->setContentsMargins(0, 0, 0, 0);
		selectionLayout->setSpacing(0);
		QVector<QCheckBox*> selectAllBoxes;

		AddCleanScopeRow(
		    *selectionLayout,
		    {"All",
		        "clean-all",
		        "Remove build and cooked outputs, dependency caches, logs, and product user state including settings and captures. "
		        "Confirmation is required.",
		        "All generated data",
		        QString()},
		    contentId,
		    selectedScopes,
		    selectAllBoxes,
		    workspaceRoot,
		    registerFocusable);

		QCheckBox* selectAllBox = selectAllBoxes.front();

		for (const QString& cleanGroup : cleanGroups)
		{
			QLabel* groupTitle = CreateSectionLabel(cleanGroup);
			groupTitle->setObjectName("CleanScopeGroupTitle");
			selectionLayout->addWidget(groupTitle);
			for (const CleanScopeUiOption& scope : cleanScopes)
			{
				if (scope.Group != cleanGroup)
				{
					continue;
				}

				AddCleanScopeRow(*selectionLayout, scope, contentId, selectedScopes, scopeBoxes, workspaceRoot, registerFocusable);
			}
		}
		cleanLayout->addWidget(selectionPanel);

		const auto commitSelection = [&settings, scopeBoxes, selectionChanged](bool bCleanAllRequested)
		{
			const QStringList selectedValues = CollectSelectedScopeValues(scopeBoxes);
			settings.SetCleanScope(bCleanAllRequested ? QStringLiteral("clean-all") : selectedValues.join(';'));
			selectionChanged();
		};

		ConnectSelectAllScopeBox(selectAllBox, scopeBoxes, layout.parentWidget(), commitSelection);
		commitSelection(selectedScopes.contains("clean-all"));
	}
}
