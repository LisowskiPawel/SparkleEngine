#include "Gui/App/LauncherSettings.h"

namespace SparkleLauncher
{
	static QString NormalizeBuildConfiguration(const QString& configuration)
	{
		const QString lowered = configuration.trimmed().toLower();
		if (lowered.startsWith("debug"))
		{
			return "debug";
		}
		if (lowered.startsWith("shipping"))
		{
			return "shipping";
		}
		return "development";
	}

	LauncherSettings::LauncherSettings(QObject* parent) :
	    QObject(parent)
	{
	}

	const QString& LauncherSettings::EditorProfile() const
	{
		static QString debugProfile = "DebugEditor";
		static QString developmentProfile = "DevelopmentEditor";
		static QString shippingProfile = "ShippingEditor";
		if (m_buildConfiguration == "debug")
		{
			return debugProfile;
		}
		if (m_buildConfiguration == "shipping")
		{
			return shippingProfile;
		}
		return developmentProfile;
	}

	const QString& LauncherSettings::GameProfile() const
	{
		static QString debugProfile = "DebugGame";
		static QString developmentProfile = "DevelopmentGame";
		static QString shippingProfile = "ShippingGame";
		if (m_buildConfiguration == "debug")
		{
			return debugProfile;
		}
		if (m_buildConfiguration == "shipping")
		{
			return shippingProfile;
		}
		return developmentProfile;
	}

	void LauncherSettings::SetShaderSymbols(bool bEnabled)
	{
		AssignSetting(m_bShaderSymbols, bEnabled);
	}

	void LauncherSettings::SetCaptureProvider(const QString& value)
	{
		ExternalCaptureProvider provider;
		if (!TryParseExternalCaptureProvider(value.toStdString(), provider))
		{
			return;
		}

		AssignSetting(m_captureProvider, provider);
	}

	void LauncherSettings::SetGpuCrashMode(const QString& mode)
	{
		if (mode != "off" && mode != "aftermath")
		{
			return;
		}
		const auto selected = mode == "off" ? RhiGpuCrashMode::Off : RhiGpuCrashMode::Aftermath;
		AssignSetting(m_gpuCrashMode, selected);
	}

	void LauncherSettings::SetShaderExportPath(const QString& path)
	{
		AssignSetting(m_shaderExportPath, path);
	}

	void LauncherSettings::SetGpuCrashInspectorPath(const QString& path)
	{
		AssignSetting(m_gpuCrashInspectorPath, path);
	}

	void LauncherSettings::SetRunMode(const QString& runMode)
	{
		const QString normalized = runMode.trimmed().toLower() == "game" ? QStringLiteral("game") : QStringLiteral("editor");
		AssignSetting(m_runMode, normalized);
	}

	void LauncherSettings::SetBuildConfiguration(const QString& configuration)
	{
		const QString normalized = NormalizeBuildConfiguration(configuration);
		AssignSetting(m_buildConfiguration, normalized);
	}

	void LauncherSettings::SetWorkspaceIde(const QString& ide)
	{
		AssignSetting(m_workspaceIde, ide);
	}

	void LauncherSettings::SetWorkspaceCompiler(const QString& compiler)
	{
		AssignSetting(m_workspaceCompiler, compiler);
	}

	void LauncherSettings::SetBuildScopes(const QString& scopes)
	{
		AssignSetting(m_buildScopes, scopes);
	}

	void LauncherSettings::SetCookScopes(const QString& scopes)
	{
		AssignSetting(m_cookScopes, scopes);
	}

	void LauncherSettings::SetShaderBackend(const QString& backend)
	{
		const QString normalized = backend.trimmed().toLower() == "slang" ? QStringLiteral("slang") : QStringLiteral("dxc");
		AssignSetting(m_shaderBackend, normalized);
	}

	void LauncherSettings::SetGraphicsApi(const QString& graphicsApi)
	{
		const QString normalized = graphicsApi.trimmed().toLower() == "vulkan" ? QStringLiteral("vulkan") : QStringLiteral("d3d12");
		AssignSetting(m_graphicsApi, normalized);
	}

	void LauncherSettings::SetCleanScope(const QString& scope)
	{
		AssignSetting(m_cleanScope, scope);
	}

}
