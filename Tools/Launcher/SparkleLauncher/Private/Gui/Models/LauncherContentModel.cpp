#include "Gui/Models/LauncherContentModel.h"

#include "SparkleLauncher/ProjectResolver.h"

#include <string>

namespace SparkleLauncher
{
	static bool ContentMatches(const LauncherContentSummary& left, const LauncherContentSummary& right)
	{
		return left.Id == right.Id && left.DisplayName == right.DisplayName && left.RootPath == right.RootPath;
	}

	LauncherContentModel::LauncherContentModel(QObject* parent) :
	    QObject(parent)
	{
	}

	const QString& LauncherContentModel::ContentId() const
	{
		return m_content.Id;
	}

	const LauncherContentSummary* LauncherContentModel::Content() const
	{
		return m_bHasContent ? &m_content : nullptr;
	}

	void LauncherContentModel::Refresh(const std::filesystem::path& workspaceRoot)
	{
		std::string errorMessage;
		const std::optional<LauncherProject> discoveredContent = ResolveLauncherProject(workspaceRoot, errorMessage);

		LauncherContentSummary refreshedContent;
		const bool bHasRefreshedContent = discoveredContent.has_value();
		if (bHasRefreshedContent)
		{
			const LauncherProject& content = *discoveredContent;
			const QString projectName = QString::fromStdString(content.Id);
			refreshedContent = {projectName, projectName, content.RootPath};
		}

		const bool bContentChangedHasContent = m_bHasContent != bHasRefreshedContent;
		const bool bContentPresent = !bContentChangedHasContent && (m_bHasContent);
		const bool bContentChangedContentMatches = bContentPresent && (!ContentMatches(m_content, refreshedContent));
		const bool bContentChanged = bContentChangedHasContent || bContentChangedContentMatches;
		m_content = std::move(refreshedContent);
		m_bHasContent = bHasRefreshedContent;

		if (bContentChanged)
		{
			emit ContentChanged();
		}

		if (!discoveredContent.has_value() && errorMessage.empty())
		{
			errorMessage = "The workspace content root could not be discovered.";
		}
		if (!errorMessage.empty())
		{
			emit ContentDiscoveryFailed(QString::fromStdString(errorMessage));
		}
	}
}
