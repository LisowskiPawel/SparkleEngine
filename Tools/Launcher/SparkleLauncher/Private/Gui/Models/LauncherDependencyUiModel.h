#pragma once

#include <QtCore/QString>

#include <filesystem>
#include <vector>

namespace SparkleLauncher
{
	struct ThirdPartyDependencyUiEntry
	{
		QString Id;
		QString Label;
		QString Version;
		QString Purpose;
		QString CacheDirectoryName;
		bool bRequired = false;
		bool bEnabled = false;
	};

	struct ThirdPartyDependencyUiStatus
	{
		QString Text;
		QString Detail;
		QString State;
		bool bSynced = false;
	};

	const std::vector<ThirdPartyDependencyUiEntry>& GetTrackedThirdPartyDependencies();
	ThirdPartyDependencyUiStatus BuildThirdPartyDependencyStatus(const ThirdPartyDependencyUiEntry& dependency, const std::filesystem::path& dependencyCachePath);
}
