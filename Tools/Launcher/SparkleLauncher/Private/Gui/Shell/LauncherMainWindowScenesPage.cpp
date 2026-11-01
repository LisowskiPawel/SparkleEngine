#include "LauncherMainWindow.h"

#include "LauncherSceneWidgets.h"
#include "LauncherUiDesign.h"

#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace SparkleLauncher
{
	void LauncherMainWindow::AddSceneSelection(QVBoxLayout& layout)
	{
		QWidget* sceneSelectionBody = new QWidget(this);
		sceneSelectionBody->setObjectName("SceneSelectionBody");
		sceneSelectionBody->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
		QVBoxLayout* bodyLayout = new QVBoxLayout(sceneSelectionBody);
		bodyLayout->setContentsMargins(LauncherUi::Scenes::BodyLeft, LauncherUi::Scenes::BodyTop, LauncherUi::Scenes::BodyRight, LauncherUi::Scenes::BodyBottom);
		bodyLayout->setSpacing(LauncherUi::Scenes::SectionSpacing);

		const QString heroTitle = "Explore Sparkle";
		const QString heroDetail = QStringLiteral("Choose a scene and open Editor or Game. Sync, Build and Cook are optional developer tools.");
		layout.addWidget(CreateSceneHeroCard(m_workspaceRoot, heroTitle, heroDetail, "neutral", nullptr, nullptr, "showcase-hero.png", this));

		AddLevelContentGroups(*bodyLayout);
		layout.addWidget(sceneSelectionBody);
	}
}
