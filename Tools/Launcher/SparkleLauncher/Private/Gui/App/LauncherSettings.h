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

		const QString& RunMode() const;
		const QString& BuildConfiguration() const;
		const QString& EditorProfile() const;
		const QString& GameProfile() const;
		const QString& WorkspaceIde() const;
		const QString& WorkspaceCompiler() const;
		const QString& BuildScopes() const;
		const QString& CookScopes() const;
		const QString& ShaderBackend() const;
		const QString& GraphicsApi() const;
		ExternalCaptureProvider CaptureProvider() const;
		RhiGpuCrashMode GpuCrashMode() const;
		const QString& ShaderExportPath() const;
		const QString& GpuCrashInspectorPath() const;
		const QString& CleanScope() const;

	public slots:
		void SetRunMode(const QString& runMode);
		void SetBuildConfiguration(const QString& configuration);
		void SetWorkspaceIde(const QString& ide);
		void SetWorkspaceCompiler(const QString& compiler);
		void SetBuildScopes(const QString& scopes);
		void SetCookScopes(const QString& scopes);
		void SetShaderBackend(const QString& backend);
		void SetGraphicsApi(const QString& graphicsApi);
		void SetCaptureProvider(const QString& provider);
		void SetGpuCrashMode(const QString& mode);
		void SetShaderExportPath(const QString& path);
		void SetGpuCrashInspectorPath(const QString& path);
		void SetCleanScope(const QString& scope);

	signals:
		void SettingsChanged();

	private:
		QString m_runMode = "editor";

		QString m_buildConfiguration = "development";
		QString m_workspaceIde = "rider";
		QString m_workspaceCompiler = "msvc";
		QString m_buildScopes;
		QString m_cookScopes;
		QString m_shaderBackend = "dxc";
		QString m_graphicsApi = "d3d12";
		ExternalCaptureProvider m_captureProvider = ExternalCaptureProvider::None;
		RhiGpuCrashMode m_gpuCrashMode = RhiGpuCrashMode::Off;
		QString m_shaderExportPath;
		QString m_gpuCrashInspectorPath;
		QString m_cleanScope = "cooked";
	};
}
