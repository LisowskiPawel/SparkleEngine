#include "Gui/Models/LauncherDependencyUiModel.h"

#include "SparkleLauncher/SourceDependencyState.h"

#include <string_view>
#include <system_error>

namespace SparkleLauncher
{
	static QString ToQString(std::string_view text)
	{
		return QString::fromStdString(std::string(text));
	}

	static ThirdPartyDependencyUiEntry ToUiDependency(const SourceDependencyEntry& dependency)
	{
		const std::string purpose = dependency.ExpectedGitRevision.empty() ? dependency.Purpose : dependency.Purpose + "\nSource pin: " + dependency.ExpectedGitRevision;
		return {
		    ToQString(dependency.Id),
		    ToQString(dependency.Label),
		    ToQString(dependency.Version),
		    ToQString(purpose),
		    ToQString(dependency.CacheDirectoryName),
		    dependency.bRequired,
		    dependency.bEnabled,
		};
	}

	static SourceDependencyValidation ValidateDependency(const ThirdPartyDependencyUiEntry& dependency, const std::filesystem::path& dependencyCachePath)
	{
		if (const SourceDependencyEntry* sourceDependency = FindSourceDependency(dependency.Id.toStdString()))
		{
			return InspectSourceDependencyAvailability(*sourceDependency, dependencyCachePath);
		}

		SourceDependencyValidation validation;
		validation.CachePath = dependencyCachePath / dependency.CacheDirectoryName.toStdString();
		validation.MissingRelativePaths.push_back(dependency.CacheDirectoryName.toStdString());
		return validation;
	}

	const std::vector<ThirdPartyDependencyUiEntry>& GetTrackedThirdPartyDependencies()
	{
		static const std::vector<ThirdPartyDependencyUiEntry> dependencies = []
		{
			std::vector<ThirdPartyDependencyUiEntry> entries;
			for (const SourceDependencyEntry& dependency : GetSourceDependencies())
			{
				entries.push_back(ToUiDependency(dependency));
			}
			return entries;
		}();

		return dependencies;
	}

	ThirdPartyDependencyUiStatus BuildThirdPartyDependencyStatus(const ThirdPartyDependencyUiEntry& dependency, const std::filesystem::path& dependencyCachePath)
	{
		const SourceDependencyValidation validation = ValidateDependency(dependency, dependencyCachePath);
		const QString description = dependency.Purpose.isEmpty() ? dependency.Version : QStringLiteral("%1 \u00b7 %2").arg(dependency.Version, dependency.Purpose);
		const QString detail = validation.FailureReason.empty() ? description : ToQString(validation.FailureReason);
		std::error_code errorCode;
		const bool bHasCache = std::filesystem::exists(validation.CachePath, errorCode);
		const QString unavailableState = !validation.FailureReason.empty() ? "Unavailable" : (bHasCache ? "Incomplete" : "Missing");
		return validation.bReady ? ThirdPartyDependencyUiStatus{"Available", detail, "ok", bHasCache} : ThirdPartyDependencyUiStatus{unavailableState, detail, "warning", bHasCache};
	}
}
