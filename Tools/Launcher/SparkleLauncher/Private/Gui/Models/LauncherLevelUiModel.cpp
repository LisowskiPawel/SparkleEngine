#include "Gui/Models/LauncherLevelUiModel.h"

#include "Gui/Models/LauncherContentModel.h"
#include "Cook/CookedContentReadiness.h"
#include "Levels/LevelPreparation.h"
#include "Workspace/LauncherProductPaths.h"

#include "Core/Public/Diagnostics/Error.h"
#include "ProjectLevelCatalog.h"

#include <QtCore/QStringList>

#include <filesystem>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace SparkleLauncher
{
	class LauncherLevelUiModelBuilder final
	{
	public:
		explicit LauncherLevelUiModelBuilder(const LauncherContentSummary& content) noexcept :
		    m_content(content)
		{
		}

		LauncherLevelUiModel Build()
		{
			try
			{
				m_catalog = ProjectLevelCatalogFile::Load(m_content.RootPath);
				m_model.bLoaded = true;
			}
			catch (const Diagnostics::Error& error)
			{
				m_model.LoadError = QString::fromStdString(error.what());
				return std::move(m_model);
			}

			const auto root = m_content.RootPath.parent_path().parent_path();
			const auto cooked = InspectCookedContentReadiness(root, m_content.Id.toStdString());
			std::error_code sourceError;
			const bool bHasSourceTools = std::filesystem::is_regular_file(root / "Tools/Launcher/SparkleLauncher/Scripts/SyncAssetPack.cmake", sourceError) && !sourceError;
			m_model.Levels.reserve(static_cast<qsizetype>(m_catalog.levels.size()));
			for (const ProjectLevelCatalogEntry& level : m_catalog.levels)
			{
				m_model.Levels.push_back(BuildLevel(level, cooked, bHasSourceTools));
			}
			return std::move(m_model);
		}

	private:
		LauncherLevelUiEntry BuildLevel(const ProjectLevelCatalogEntry& level, const CookedContentReadiness& cooked, bool bHasSourceTools) const
		{
			const auto pack = level.assetPackId.empty() ? m_catalog.assetPacks.end() : m_catalog.assetPacks.find(level.assetPackId);
			const ProjectAssetPack* assetPack = pack == m_catalog.assetPacks.end() ? nullptr : &pack->second;
			const LevelPreparation preparation = InspectLevelPreparation(m_catalog, level, cooked, bHasSourceTools);

			LauncherLevelUiEntry entry{
			    .Id = QString::fromStdString(level.id),
			    .DisplayName = DisplayNameOrId(level.displayName, level.id),
			    .Description = QString::fromStdString(level.description),
			    .ThumbnailPath = QString::fromStdWString(level.thumbnailPath.wstring()),
			    .Detail = BuildLevelDetail(level, assetPack),
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

		QString BuildLevelDetail(const ProjectLevelCatalogEntry& level, const ProjectAssetPack* pack) const
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
				const std::uintmax_t archiveBytes = ArchiveChainBytes(pack);
				if (archiveBytes > 0)
				{
					traits.push_back(FormatArchiveSize(archiveBytes) + " download");
				}
			}

			return traits.empty() ? QStringLiteral("Workspace map") : traits.join("  ·  ");
		}

		std::uintmax_t ArchiveChainBytes(const ProjectAssetPack* pack) const
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

				const auto parent = m_catalog.assetPacks.find(pack->parentPackId);
				pack = parent == m_catalog.assetPacks.end() ? nullptr : &parent->second;
			}
			return total;
		}

		QString ResolveLevelStatus(const LauncherLevelUiEntry& level) const
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

		QString ResolveLevelState(const LauncherLevelUiEntry& level) const
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

		QString DisplayNameOrId(std::string_view displayName, std::string_view id) const { return QString::fromStdString(std::string(displayName.empty() ? id : displayName)); }

		QString FormatArchiveSize(std::uintmax_t byteCount) const
		{
			constexpr double bytesPerGibibyte = 1024.0 * 1024.0 * 1024.0;
			constexpr double bytesPerMebibyte = 1024.0 * 1024.0;
			std::ostringstream stream;
			stream << std::fixed << std::setprecision(2);
			if (byteCount >= static_cast<std::uintmax_t>(bytesPerGibibyte))
			{
				stream << static_cast<double>(byteCount) / bytesPerGibibyte << " GiB";
			}
			else
			{
				stream << static_cast<double>(byteCount) / bytesPerMebibyte << " MiB";
			}
			return QString::fromStdString(stream.str());
		}

		const LauncherContentSummary& m_content;
		ProjectLevelCatalog m_catalog;
		LauncherLevelUiModel m_model;
	};

	LauncherLevelUiModel LauncherLevelUiModel::Build(const LauncherContentSummary& content)
	{
		return LauncherLevelUiModelBuilder(content).Build();
	}
}
