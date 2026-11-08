#pragma once

#include <QtCore/QString>
#include <QtCore/QVector>

namespace SparkleLauncher
{
	struct LauncherContentSummary;

	struct LauncherLevelUiEntry final
	{
		QString Id;
		QString DisplayName;
		QString Description;
		QString ThumbnailPath;
		QString Detail;
		QString Status;
		QString State;
		QString SourcePageUrl;
		QString UnsupportedReason;
		bool bSelected = false;
		bool bSourceReady = false;
		bool bGameSupported = true;
		bool bReady = false;
		bool bCanSelect = true;
		bool bCanPrepare = false;
		bool bCanSync = false;
	};

	struct LauncherLevelUiModel final
	{
		bool bLoaded = false;
		QString LoadError;
		QVector<LauncherLevelUiEntry> Levels;

		static LauncherLevelUiModel Build(const LauncherContentSummary& content);
	};
}
