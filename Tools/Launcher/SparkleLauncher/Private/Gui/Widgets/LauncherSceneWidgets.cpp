#include "LauncherSceneWidgets.h"

#include "LauncherLayoutWidgets.h"
#include "LauncherUiDesign.h"

#include "ToolOutputPaths.h"

#include <QtCore/QCoreApplication>
#include <QtCore/Qt>
#include <QtGui/QPixmap>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

#include <array>
#include <string>
#include <system_error>

namespace SparkleLauncher
{
	static constexpr int kSpaceSmall = LauncherUi::Space::Small;

	static std::filesystem::path FindSceneArtwork(const std::filesystem::path& workspaceRoot, const QString& fileName)
	{
		if (fileName.isEmpty())
		{
			return {};
		}

		const std::string assetName = fileName.toStdString();
		const std::filesystem::path applicationVisualPath = std::filesystem::path(QCoreApplication::applicationDirPath().toStdWString()) / "Visuals" / assetName;
		const SparkleTools::ToolOutputPaths outputs = SparkleTools::ResolveToolOutputPaths(workspaceRoot);

		const std::array<std::filesystem::path, 4> candidates = {
		    workspaceRoot / "Tools" / "Launcher" / "SparkleLauncher" / "Assets" / "Visuals" / assetName,
		    applicationVisualPath,
		    outputs.DiagnosticsRoot / "launcher-visual-assets" / assetName,
		    outputs.DiagnosticsRoot / "launcher-visual-assets" / (std::filesystem::path(assetName).stem().string() + ".png")};

		for (const std::filesystem::path& candidate : candidates)
		{
			std::error_code errorCode;
			if (std::filesystem::is_regular_file(candidate, errorCode) && !errorCode)
			{
				return candidate;
			}
		}

		return {};
	}

	QFrame* CreateSceneHeroCard(
	    const std::filesystem::path& workspaceRoot,
	    const QString& status,
	    const QString& detail,
	    const QString& state,
	    QWidget* primaryAction,
	    QWidget* secondaryAction,
	    const QString& artworkFileName,
	    QWidget* parent)
	{
		QPixmap heroPixmap;
		const std::filesystem::path artworkPath = FindSceneArtwork(workspaceRoot, artworkFileName);
		if (!artworkPath.empty())
		{
			heroPixmap.load(QString::fromStdWString(artworkPath.wstring()));
		}

		SceneHeroCardWidget* card = new SceneHeroCardWidget(heroPixmap, parent);
		card->setObjectName("CommandHeroCard");
		card->setProperty("State", state);

		QWidget* copyPane = new QWidget(card);
		copyPane->setObjectName("CommandHeroCopyPane");
		QVBoxLayout* layout = new QVBoxLayout(copyPane);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(LauncherUi::Hero::CopySpacing);

		QLabel* title = new QLabel(status, card);
		title->setObjectName("CommandHeroTitle");
		layout->addWidget(title);

		QLabel* body = new QLabel(detail, card);
		body->setObjectName("CommandHeroText");
		body->setWordWrap(true);
		layout->addWidget(body);

		QHBoxLayout* actionRow = new QHBoxLayout();
		actionRow->setContentsMargins(0, LauncherUi::Hero::ActionTopMargin, 0, 0);
		actionRow->setSpacing(kSpaceSmall);
		if (primaryAction != nullptr)
		{
			primaryAction->setParent(copyPane);
			actionRow->addWidget(primaryAction, 0, Qt::AlignLeft);
		}
		if (secondaryAction != nullptr)
		{
			secondaryAction->setParent(copyPane);
			actionRow->addWidget(secondaryAction, 0, Qt::AlignLeft);
		}
		actionRow->addStretch(1);
		layout->addLayout(actionRow);
		layout->addStretch(1);

		card->SetCopyPane(copyPane);
		return card;
	}

}
