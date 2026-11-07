#include "PCH.h"

#include "Viewport/EditorViewportSettings.h"
#include "Viewport/EditorViewportSettingsSerialization.h"

#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/ProductUserStatePaths.h"

#include <algorithm>
#include <fstream>
#include <system_error>
#include <utility>

EditorViewportSettings::EditorViewportSettings() :
    EditorViewportSettings(GetDefaultPath())
{
}

EditorViewportSettings::EditorViewportSettings(std::filesystem::path path) :
    m_path(std::move(path))
{
	(void) Reload();
}

std::filesystem::path EditorViewportSettings::GetDefaultPath()
{
	const Filesystem::ProductUserStatePaths& userState = Filesystem::GetProductUserStatePaths();
	return userState.SettingsRoot / "EditorViewport.ini";
}

void EditorViewportSettings::SanitizeExposure(ViewportExposureOverrides& exposure) noexcept
{
	if (exposure.Mode != EngineExposureMode::Manual && exposure.Mode != EngineExposureMode::Automatic)
	{
		exposure.Mode = EngineExposureMode::Automatic;
	}
	if (exposure.MeteringMethod != EngineExposureMeteringMethod::Histogram && exposure.MeteringMethod != EngineExposureMeteringMethod::DownsamplePyramid)
	{
		exposure.MeteringMethod = EngineExposureMeteringMethod::Histogram;
	}
	exposure.ManualExposure = (std::max) (exposure.ManualExposure, 0.0f);
	exposure.Compensation = std::clamp(exposure.Compensation, -16.0f, 16.0f);
	exposure.TargetLuminance = (std::max) (exposure.TargetLuminance, 0.0001f);
	exposure.Minimum = (std::max) (exposure.Minimum, 0.0f);
	exposure.Maximum = (std::max) (exposure.Maximum, exposure.Minimum);
	exposure.AdaptationSpeedUp = (std::max) (exposure.AdaptationSpeedUp, 0.0f);
	exposure.AdaptationSpeedDown = (std::max) (exposure.AdaptationSpeedDown, 0.0f);
}

void EditorViewportSettings::Sanitize(EditorViewportSettingsState& state) noexcept
{
	state.Navigation.MinimumMoveSpeedMetersPerSecond = (std::max) (state.Navigation.MinimumMoveSpeedMetersPerSecond, 0.0001f);
	state.Navigation.MaximumMoveSpeedMetersPerSecond = (std::max) (state.Navigation.MaximumMoveSpeedMetersPerSecond, state.Navigation.MinimumMoveSpeedMetersPerSecond);

	state.Navigation.MoveSpeedMetersPerSecond = std::clamp(
	    state.Navigation.MoveSpeedMetersPerSecond,
	    state.Navigation.MinimumMoveSpeedMetersPerSecond,
	    state.Navigation.MaximumMoveSpeedMetersPerSecond);

	state.Navigation.RotationSpeedDegreesPerPixel = std::clamp(state.Navigation.RotationSpeedDegreesPerPixel, 0.001f, 10.0f);
	state.Navigation.SprintMultiplier = (std::max) (state.Navigation.SprintMultiplier, 1.0f);
	state.OrthographicHeightMeters = std::clamp(state.OrthographicHeightMeters, 0.001f, 1000000.0f);
	if (state.ProjectionKind != CameraProjectionKind::Perspective && state.ProjectionKind != CameraProjectionKind::Orthographic)
	{
		state.ProjectionKind = CameraProjectionKind::Perspective;
	}
	SanitizeExposure(state.Exposure);
}

bool EditorViewportSettings::SetMoveSpeed(float speedMetersPerSecond) noexcept
{
	speedMetersPerSecond = std::clamp(speedMetersPerSecond, m_state.Navigation.MinimumMoveSpeedMetersPerSecond, m_state.Navigation.MaximumMoveSpeedMetersPerSecond);
	if (speedMetersPerSecond == m_state.Navigation.MoveSpeedMetersPerSecond)
	{
		return true;
	}
	m_state.Navigation.MoveSpeedMetersPerSecond = speedMetersPerSecond;
	return Save();
}

bool EditorViewportSettings::SetRotationSpeed(float degreesPerPixel) noexcept
{
	degreesPerPixel = std::clamp(degreesPerPixel, 0.001f, 10.0f);
	if (degreesPerPixel == m_state.Navigation.RotationSpeedDegreesPerPixel)
	{
		return true;
	}
	m_state.Navigation.RotationSpeedDegreesPerPixel = degreesPerPixel;
	return Save();
}

bool EditorViewportSettings::SetInvertY(bool invertY) noexcept
{
	if (invertY == m_state.Navigation.InvertY)
	{
		return true;
	}
	m_state.Navigation.InvertY = invertY;
	return Save();
}

bool EditorViewportSettings::SetProjectionKind(CameraProjectionKind projectionKind) noexcept
{
	if (projectionKind != CameraProjectionKind::Perspective && projectionKind != CameraProjectionKind::Orthographic)
	{
		projectionKind = CameraProjectionKind::Perspective;
	}
	if (projectionKind == m_state.ProjectionKind)
	{
		return true;
	}
	m_state.ProjectionKind = projectionKind;
	return Save();
}

bool EditorViewportSettings::SetOrthographicHeight(float heightMeters) noexcept
{
	heightMeters = std::clamp(heightMeters, 0.001f, 1000000.0f);
	if (heightMeters == m_state.OrthographicHeightMeters)
	{
		return true;
	}
	m_state.OrthographicHeightMeters = heightMeters;
	return Save();
}

bool EditorViewportSettings::SetExposureOverrides(ViewportExposureOverrides overrides) noexcept
{
	SanitizeExposure(overrides);
	if (overrides == m_state.Exposure)
	{
		return true;
	}
	m_state.Exposure = overrides;
	return Save();
}

bool EditorViewportSettings::Reload() noexcept
{
	EditorViewportSettingsState loaded;
	std::ifstream input(m_path);
	if (input.is_open())
	{
		EditorViewportSettingsSerialization::Read(input, loaded);
	}
	Sanitize(loaded);
	m_state = loaded;
	return true;
}

bool EditorViewportSettings::Save() const noexcept
{
	std::error_code errorCode;
	std::filesystem::create_directories(m_path.parent_path(), errorCode);
	if (errorCode)
	{
		return false;
	}

	std::ofstream output(m_path, std::ios::trunc);
	if (!output.is_open())
	{
		return false;
	}

	EditorViewportSettingsSerialization::Write(output, m_state);
	return output.good();
}
