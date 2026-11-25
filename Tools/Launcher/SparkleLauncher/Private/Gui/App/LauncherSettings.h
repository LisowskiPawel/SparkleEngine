#pragma once

#include "RHI/Public/Diagnostics/ExternalCaptureProvider.h"
#include "RHI/Public/Diagnostics/RhiGpuCrash.h"

#include <QtCore/QObject>
#include <QtCore/QString>

namespace SparkleLauncher
{
	class LauncherSettings final : public QObject
	{
		Q_OBJECT

	public:
		explicit LauncherSettings(QObject* parent = nullptr);

		const QString& RunMode() const { return m_runMode; }

		const QString& BuildConfiguration() const { return m_buildConfiguration; }

		const QString& EditorProfile() const;
		const QString& GameProfile() const;

		const QString& WorkspaceIde() const { return m_workspaceIde; }

		const QString& WorkspaceCompiler() const { return m_workspaceCompiler; }

		const QString& BuildScopes() const { return m_buildScopes; }

		const QString& CookScopes() const { return m_cookScopes; }

		const QString& ShaderBackend() const { return m_shaderBackend; }

		bool ShaderSymbols() const { return m_bShaderSymbols; }

		const QString& GraphicsApi() const { return m_graphicsApi; }

		ExternalCaptureProvider CaptureProvider() const { return m_captureProvider; }

		RhiGpuCrashMode GpuCrashMode() const { return m_gpuCrashMode; }

		const QString& ShaderExportPath() const { return m_shaderExportPath; }

		const QString& GpuCrashInspectorPath() const { return m_gpuCrashInspectorPath; }

		const QString& CleanScope() const { return m_cleanScope; }

	public slots:
		void SetRunMode(const QString& runMode);
		void SetBuildConfiguration(const QString& configuration);
		void SetWorkspaceIde(const QString& ide);
		void SetWorkspaceCompiler(const QString& compiler);
		void SetBuildScopes(const QString& scopes);
		void SetCookScopes(const QString& scopes);
		void SetShaderBackend(const QString& backend);
		void SetShaderSymbols(bool bEnabled);
		void SetGraphicsApi(const QString& graphicsApi);
		void SetCaptureProvider(const QString& provider);
		void SetGpuCrashMode(const QString& mode);
		void SetShaderExportPath(const QString& path);
		void SetGpuCrashInspectorPath(const QString& path);
		void SetCleanScope(const QString& scope);

	signals:
		void SettingsChanged();

	private:
		template <typename TValue> void AssignSetting(TValue& setting, const TValue& value)
		{
			if (setting != value)
			{
				setting = value;
				emit SettingsChanged();
			}
		}

		QString m_runMode = "editor";

		QString m_buildConfiguration = "development";
		QString m_workspaceIde = "rider";
		QString m_workspaceCompiler = "msvc";
		QString m_buildScopes;
		QString m_cookScopes;
		QString m_shaderBackend = "dxc";
		bool m_bShaderSymbols = false;
		QString m_graphicsApi = "d3d12";
		ExternalCaptureProvider m_captureProvider = ExternalCaptureProvider::None;
		RhiGpuCrashMode m_gpuCrashMode = RhiGpuCrashMode::Off;
		QString m_shaderExportPath;
		QString m_gpuCrashInspectorPath;
		QString m_cleanScope = "cooked";
	};
}
