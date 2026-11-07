#include "Gui/Models/LauncherExternalCaptureUiModel.h"
#include "ExternalCapture/ExternalCaptureDiscovery.h"

namespace SparkleLauncher
{
	static LauncherSelectionOption BuildExternalCaptureOption(ExternalCaptureProvider provider, const ExternalCaptureAvailability& availability)
	{
		QString name = QString::fromUtf8(ExternalCaptureProviderDisplayName(provider).data());
		QString detail = QString::fromStdString(availability.Detail);
		if (provider == ExternalCaptureProvider::NsightGraphics)
		{
			detail.prepend("Experimental SDK. ");
		}

		return {name, QString::fromUtf8(ExternalCaptureProviderToString(provider).data()), detail, availability.Available(), availability.bSupported};
	}

	QVector<LauncherSelectionOption> BuildExternalCaptureOptions(std::string_view api, std::string_view profile)
	{
		QVector<LauncherSelectionOption> options;
		for (auto provider : ExternalCaptureProviders)
		{
			const auto availability = InspectExternalCaptureProvider(provider, api, profile);
			options.push_back(BuildExternalCaptureOption(provider, availability));
		}

		return options;
	}
}
