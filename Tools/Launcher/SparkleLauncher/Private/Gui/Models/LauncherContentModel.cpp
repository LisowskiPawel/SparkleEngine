#include "Gui/Models/LauncherContentModel.h"

#include "SparkleLauncher/ProjectResolver.h"

#include <string>
#include <optional>
#include <utility>

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
		return m_content.Id.isEmpty() ? nullptr : &m_content;
	}

	void LauncherContentModel::Refresh(const std::filesystem::path& workspaceRoot)
	{
		std::string errorMessage;
		const std::optional<LauncherProject> discoveredContent = ResolveLauncherProject(workspaceRoot, errorMessage);

		LauncherContentSummary refreshedContent;
		if (discoveredContent.has_value())
		{
			const LauncherProject& content = *discoveredContent;
			const QString projectName = QString::fromStdString(content.Id);
			refreshedContent = {projectName, projectName, content.RootPath};
		}

		const bool bContentChanged = !ContentMatches(m_content, refreshedContent);
		m_content = std::move(refreshedContent);

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
