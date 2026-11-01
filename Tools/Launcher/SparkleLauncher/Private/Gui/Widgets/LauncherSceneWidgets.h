#pragma once

#include <QtCore/QString>

#include <filesystem>

class QFrame;
class QWidget;

namespace SparkleLauncher
{
	QFrame* CreateSceneHeroCard(
	    const std::filesystem::path& workspaceRoot,
	    const QString& status,
	    const QString& detail,
	    const QString& state,
	    QWidget* primaryAction,
	    QWidget* secondaryAction,
	    const QString& artworkFileName,
	    QWidget* parent);
}
