#pragma once

#include <QtCore/QAbstractListModel>
#include <QtCore/QVector>
#include <QtGui/QPixmap>

#include <cstdint>

namespace SparkleLauncher
{
	struct LauncherContentSummary;
	class LauncherSettings;

	enum class LauncherLevelAction : std::uint8_t
	{
		None,
		Run,
		Sync,
		Details,
		Source,
		CopyDetails,
	};

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
		QString ActionText;
		QString ActionToolTip;
		QString ArtworkIdentity;
		QPixmap Artwork;
		LauncherLevelAction Action = LauncherLevelAction::None;
		bool bSelected = false;
		bool bSourceReady = false;
		bool bGameSupported = true;
		bool bReady = false;
		bool bCanSelect = true;
		bool bCanPrepare = false;
		bool bCanSync = false;
	};

	class LauncherLevelUiModel final : public QAbstractListModel
	{
	public:
		explicit LauncherLevelUiModel(QObject* parent = nullptr);
		void Refresh(const LauncherContentSummary* content, const LauncherSettings& settings, const QString& openingLevelId);
		int rowCount(const QModelIndex& parent = QModelIndex()) const override;
		QVariant data(const QModelIndex& index, int role) const override;
		const LauncherLevelUiEntry* Level(const QModelIndex& index) const;

		const QVector<LauncherLevelUiEntry>& Levels() const { return m_levels; }

		const QString& ContentId() const { return m_contentId; }

		const QString& Error() const { return m_error; }

	private:
		QVector<LauncherLevelUiEntry> m_levels;
		QString m_contentId;
		QString m_error;
	};
}
