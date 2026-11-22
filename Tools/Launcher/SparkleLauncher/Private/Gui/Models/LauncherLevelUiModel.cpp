#include "Gui/Models/LauncherLevelUiModel.h"

#include "Gui/Models/LauncherContentModel.h"
#include "Gui/App/LauncherSettings.h"
#include "Cook/CookedContentReadiness.h"
#include "Levels/LevelPreparation.h"
#include "Workspace/LauncherProductPaths.h"

#include "Core/Public/Diagnostics/Error.h"
#include "ProjectLevelCatalog.h"

#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>
#include <QtGui/QPen>

#include <algorithm>

#include <filesystem>
#include <limits>
#include <string>
#include <system_error>
#include <utility>

namespace SparkleLauncher
{
	static QString FormatArchiveSize(std::uintmax_t byteCount)
	{
		constexpr double bytesPerGibibyte = 1024.0 * 1024.0 * 1024.0;
		constexpr double bytesPerMebibyte = 1024.0 * 1024.0;
		return byteCount >= bytesPerGibibyte ? QString::number(byteCount / bytesPerGibibyte, 'f', 2) + " GiB" : QString::number(byteCount / bytesPerMebibyte, 'f', 2) + " MiB";
	}

	static std::uintmax_t ArchiveChainBytes(const ProjectLevelCatalog& catalog, const ProjectAssetPack* pack)
	{
		std::uintmax_t total = 0;
		while (pack != nullptr)
		{
			if (pack->bDownloadSupported)
			{
				if (pack->archiveBytes > std::numeric_limits<std::uintmax_t>::max() - total)
				{
					return std::numeric_limits<std::uintmax_t>::max();
				}
				total += pack->archiveBytes;
			}
			if (pack->parentPackId.empty())
			{
				break;
			}

			const auto parent = catalog.assetPacks.find(pack->parentPackId);
			pack = parent == catalog.assetPacks.end() ? nullptr : &parent->second;
		}
		return total;
	}

	static QString BuildLevelDetail(const ProjectLevelCatalog& catalog, const ProjectLevelCatalogEntry& level, const ProjectAssetPack* pack)
	{
		QStringList traits{level.bReleaseSupported ? QStringLiteral("Release map") : QStringLiteral("Development map")};
		if (level.variantKind == "SceneVariant")
		{
			traits.push_back("variant");
		}
		else if (level.variantKind == "AddOn")
		{
			traits.push_back("add-on");
		}
		else if (level.variantKind == "Base")
		{
			traits.push_back("base map");
		}

		if (pack != nullptr)
		{
			const std::uintmax_t archiveBytes = ArchiveChainBytes(catalog, pack);
			if (archiveBytes > 0)
			{
				traits.push_back(FormatArchiveSize(archiveBytes) + " download");
			}
		}

		return traits.empty() ? QStringLiteral("Workspace map") : traits.join("  ·  ");
	}

	static QString ResolveLevelStatus(const LauncherLevelUiEntry& level)
	{
		if (!level.bGameSupported)
		{
			return QStringLiteral("Game unsupported");
		}
		if (!level.bCanSelect)
		{
			return "Unavailable";
		}
		if (level.bReady)
		{
			return "Ready";
		}
		if (level.bSelected)
		{
			return "Download required";
		}
		return level.bSourceReady ? QStringLiteral("Source ready") : QStringLiteral("Available to sync");
	}

	static QString ResolveLevelState(const LauncherLevelUiEntry& level)
	{
		if (!level.bGameSupported || !level.bCanSelect)
		{
			return "warning";
		}
		if (level.bReady)
		{
			return "ok";
		}
		if (level.bSelected)
		{
			return "warning";
		}
		return level.bSourceReady ? QStringLiteral("ok") : QStringLiteral("neutral");
	}

	static LauncherLevelUiEntry BuildLevel(const ProjectLevelCatalog& catalog, const ProjectLevelCatalogEntry& level, const CookedContentReadiness& cooked, bool bHasSourceTools)
	{
		const auto pack = level.assetPackId.empty() ? catalog.assetPacks.end() : catalog.assetPacks.find(level.assetPackId);
		const ProjectAssetPack* assetPack = pack == catalog.assetPacks.end() ? nullptr : &pack->second;
		const LevelPreparation preparation = InspectLevelPreparation(catalog, level, cooked, bHasSourceTools);

		LauncherLevelUiEntry entry{
		    .Id = QString::fromStdString(level.id),
		    .DisplayName = QString::fromStdString(level.displayName.empty() ? level.id : level.displayName),
		    .Description = QString::fromStdString(level.description),
		    .ThumbnailPath = QString::fromStdWString(level.thumbnailPath.wstring()),
		    .Detail = BuildLevelDetail(catalog, level, assetPack),
		    .SourcePageUrl = QString::fromStdString(level.sourcePageUrl.empty() && assetPack != nullptr ? assetPack->sourcePageUrl : level.sourcePageUrl),
		    .UnsupportedReason = QString::fromStdString(preparation.UnavailableReason),
		    .bSelected = level.bSelected,
		    .bSourceReady = preparation.bSourceReady,
		    .bGameSupported = preparation.bGameSupported,
		    .bReady = preparation.bReady,
		    .bCanSelect = preparation.bCanSelect,
		    .bCanPrepare = preparation.bCanPrepare,
		    .bCanSync = preparation.bCanSync};

		entry.Status = ResolveLevelStatus(entry);
		entry.State = ResolveLevelState(entry);
		return entry;
	}

	static QVector<LauncherLevelUiEntry> BuildLevels(const LauncherContentSummary& content, QString& error)
	{
		ProjectLevelCatalog catalog;
		try
		{
			catalog = LoadProjectLevelCatalog(content.RootPath);
		}
		catch (const Diagnostics::Error& loadError)
		{
			error = QString::fromStdString(loadError.what());
			return {};
		}

		const auto root = content.RootPath.parent_path().parent_path();
		const auto cooked = InspectCookedContentReadiness(root, content.Id.toStdString());
		std::error_code sourceError;
		const bool bHasSourceTools = std::filesystem::is_regular_file(root / "Tools/Launcher/SparkleLauncher/Scripts/SyncAssetPack.cmake", sourceError) && !sourceError;
		QVector<LauncherLevelUiEntry> levels;
		levels.reserve(static_cast<qsizetype>(catalog.levels.size()));
		for (const ProjectLevelCatalogEntry& level : catalog.levels)
		{
			levels.push_back(BuildLevel(catalog, level, cooked, bHasSourceTools));
		}
		return levels;
	}

	static QPixmap CreateMapPlaceholder(const LauncherLevelUiEntry& level)
	{
		QPixmap placeholder(640, 400);
		if (level.Id == "Empty")
		{
			QPainter painter(&placeholder);
			painter.fillRect(placeholder.rect(), QColor("#101310"));
			painter.setPen(QPen(QColor(118, 185, 45, 45), 1));
			const QPoint horizon(placeholder.width() / 2, placeholder.height() * 2 / 5);
			for (int x = -160; x <= placeholder.width() + 160; x += 64)
			{
				painter.drawLine(horizon, QPoint(x, placeholder.height()));
			}
			for (int y = horizon.y(); y <= placeholder.height(); y += 32)
			{
				painter.drawLine(0, y, placeholder.width(), y);
			}

			painter.setPen(QPen(QColor("#76b92d"), 4));
			const QRect front(placeholder.width() / 2 - 58, placeholder.height() / 2 - 34, 116, 92);
			const QPoint offset(38, -28);
			const QRect back = front.translated(offset);
			painter.drawRect(front);
			painter.drawRect(back);
			painter.drawLine(front.topLeft(), back.topLeft());
			painter.drawLine(front.topRight(), back.topRight());
			painter.drawLine(front.bottomLeft(), back.bottomLeft());
			painter.drawLine(front.bottomRight(), back.bottomRight());
			return placeholder;
		}

		const uint hueSeed = qHash(level.Id);
		const QColor topColor = QColor::fromHsv(static_cast<int>(hueSeed % 80u) + 75, 125, 74);
		const QColor bottomColor = QColor::fromHsv(static_cast<int>((hueSeed + 22u) % 80u) + 75, 175, 24);
		QLinearGradient gradient(0, 0, placeholder.width(), placeholder.height());
		gradient.setColorAt(0.0, topColor);
		gradient.setColorAt(1.0, bottomColor);

		QPainter painter(&placeholder);
		painter.fillRect(placeholder.rect(), gradient);
		painter.setPen(QPen(QColor(255, 255, 255, 28), 3));
		painter.drawLine(0, placeholder.height() * 3 / 4, placeholder.width(), placeholder.height() / 3);
		painter.drawEllipse(QPoint(placeholder.width() * 4 / 5, placeholder.height() / 5), 84, 84);
		painter.setPen(QPen(QColor(232, 242, 224, 115), 5));
		const QRect marker(placeholder.width() / 2 - 54, placeholder.height() / 2 - 44, 108, 88);
		painter.drawRoundedRect(marker, 8, 8);
		painter.drawLine(marker.left() + 18, marker.bottom() - 18, marker.center().x(), marker.top() + 20);
		painter.drawLine(marker.center().x(), marker.top() + 20, marker.right() - 18, marker.bottom() - 18);
		return placeholder;
	}

	static QPixmap LoadMapArtwork(const LauncherLevelUiEntry& level)
	{
		if (!level.ThumbnailPath.isEmpty() && QFileInfo::exists(level.ThumbnailPath))
		{
			QPixmap artwork(level.ThumbnailPath);
			if (!artwork.isNull())
			{
				return artwork;
			}
		}
		return CreateMapPlaceholder(level);
	}

	LauncherLevelUiModel::LauncherLevelUiModel(QObject* parent) :
	    QAbstractListModel(parent)
	{
	}

	void LauncherLevelUiModel::Refresh(const LauncherContentSummary* content, const LauncherSettings& settings, const QString& openingLevelId)
	{
		QString error;
		QVector<LauncherLevelUiEntry> levels = content != nullptr ? BuildLevels(*content, error) : QVector<LauncherLevelUiEntry>{};
		const QString contentId = content != nullptr ? content->Id : QString();
		if (content == nullptr)
		{
			error = "Workspace content is unavailable.";
		}
		for (LauncherLevelUiEntry& level : levels)
		{
			const QFileInfo artworkFile(level.ThumbnailPath);
			level.ArtworkIdentity = level.ThumbnailPath + ":" + QString::number(artworkFile.size()) + ":" + QString::number(artworkFile.lastModified().toMSecsSinceEpoch());
			const auto previous = std::find_if(m_levels.begin(), m_levels.end(), [&level](const auto& row) { return row.Id == level.Id && row.ArtworkIdentity == level.ArtworkIdentity; });
			level.Artwork = contentId == m_contentId && previous != m_levels.end() ? previous->Artwork : LoadMapArtwork(level);
			const bool bPreparing = !openingLevelId.isEmpty() && openingLevelId == level.Id;
			const bool bSourceMissing = !level.bReady && !level.bSourceReady;
			const bool bCanSync = level.bCanSync && level.bGameSupported;
			const bool bSync = bSourceMissing && bCanSync;
			const bool bDetails = !level.bGameSupported || !level.bCanSelect;
			const QString runText = settings.RunMode() == "game" ? QStringLiteral("Run") : QStringLiteral("Open");
			level.Action = !openingLevelId.isEmpty() ? LauncherLevelAction::None : bSync ? LauncherLevelAction::Sync : bDetails ? LauncherLevelAction::Details : LauncherLevelAction::Run;
			level.ActionText = bPreparing ? QStringLiteral("Opening...") : bSync ? QStringLiteral("Sync") : bDetails ? QStringLiteral("Details") : runText;
			level.ActionToolTip = bPreparing ? QStringLiteral("Waiting for this level to start.")
			    : bSync                      ? QStringLiteral("Download and verify this level's source assets.")
			    : bDetails                   ? level.UnsupportedReason
			                                 : runText + QStringLiteral(" this level. Use Build and Cook first if required output files are missing.");
		}
		const bool bSameContent = contentId == m_contentId && levels.size() == m_levels.size();
		const bool bSameRows = bSameContent && std::equal(levels.begin(), levels.end(), m_levels.begin(), [](const auto& left, const auto& right) { return left.Id == right.Id; });
		if (!bSameRows)
		{
			beginResetModel();
		}
		m_contentId = contentId;
		m_error = std::move(error);
		m_levels = std::move(levels);
		if (!bSameRows)
		{
			endResetModel();
		}
		else if (!m_levels.empty())
		{
			emit dataChanged(index(0), index(rowCount() - 1));
		}
	}

	int LauncherLevelUiModel::rowCount(const QModelIndex& parent) const
	{
		return parent.isValid() ? 0 : static_cast<int>(m_levels.size());
	}

	const LauncherLevelUiEntry* LauncherLevelUiModel::Level(const QModelIndex& index) const
	{
		return index.isValid() && index.model() == this && index.column() == 0 && index.row() >= 0 && index.row() < m_levels.size() ? &m_levels[index.row()] : nullptr;
	}

	QVariant LauncherLevelUiModel::data(const QModelIndex& index, int role) const
	{
		const LauncherLevelUiEntry* level = Level(index);
		if (level == nullptr)
		{
			return {};
		}
		if (role == Qt::DisplayRole || role == Qt::AccessibleTextRole)
		{
			return level->DisplayName;
		}
		if (role == Qt::ToolTipRole || role == Qt::AccessibleDescriptionRole)
		{
			return level->DisplayName + "\n" + level->Description + "\n" + level->Status + " - " + level->Detail + "\n" + level->ActionToolTip;
		}
		return {};
	}
}
