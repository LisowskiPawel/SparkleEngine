#include "PCH.h"
#include "RenderingSettings/EngineRenderingSettingsPersistence.h"

#include "Core/Public/Console/CVar.h"
#include "Core/Public/Console/CVarRegistry.h"
#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Paths/PackagePaths.h"
#include "Core/Public/Paths/ProductPaths.h"
#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>

std::filesystem::path EngineRenderingSettingsPersistence::GetDefaultConfigPath()
{
	const std::filesystem::path& root = Filesystem::GetWorkspaceRootPath();
	if (!Filesystem::IsSourceWorkspace(root))
	{
		const auto defaults = Filesystem::ResolvePackageFilePath(root, "Config/DefaultEngine.ini");
		if (!defaults)
		{
			throw Diagnostics::Error("The packaged rendering defaults are missing or corrupt. Extract a verified package before retrying.");
		}
		return *defaults;
	}
	return root / "Config" / "DefaultEngine.ini";
}

std::filesystem::path EngineRenderingSettingsPersistence::GetUserConfigPath()
{
	const Filesystem::ProductUserStatePaths& userState = Filesystem::GetProductUserStatePaths();
	return userState.SettingsRoot / "EngineRendering.ini";
}

std::span<const std::string_view> EngineRenderingSettingsPersistence::GetPersistedNames() noexcept
{
	static constexpr std::string_view persistedNames[] = {
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	    "r.ExternalCapture.StartupProvider",
#endif
	    "r.VSync",
	    "r.Vignette",
	    "r.ChromaticAberration",
	    "r.BackBufferFormat",
	    "r.PreferHighPerformanceAdapter",
	    "r.ToneMapper",
	    "r.Exposure.Mode",
	    "r.Exposure.MeteringMethod",
	    "r.OutputColorEncoding",
	    "r.Exposure.Manual",
	    "r.Exposure.Compensation",
	    "r.Exposure.TargetLuminance",
	    "r.Exposure.Min",
	    "r.Exposure.Max",
	    "r.Exposure.AdaptationSpeedUp",
	    "r.Exposure.AdaptationSpeedDown",
	    "r.MeshAutoBatching",
	    "r.Upscaler.Provider",
	    "r.Upscaler.QualityMode",
	    "r.RayReconstruction.Mode",
	    "r.GBuffer.Algorithm",
	    "r.RayTracing.Tlas.Refit",
	    "r.RayTracing.PreferPtlas",
	    "r.RayTracing.Ptlas.PartitionsPerAxis",
	    "r.RayTracing.Ptlas.PartitionUpdateMode",
	    "r.RayTracing.Ptlas.MarkAllDynamicInPartition",
	    "r.RayTracing.Ptlas.ModeChangeDistance"};

	return persistedNames;
}

void EngineRenderingSettingsPersistence::Apply()
{
	ApplyFile(GetDefaultConfigPath());
	ApplyFile(GetUserConfigPath());
}

void EngineRenderingSettingsPersistence::ApplyFile(const std::filesystem::path& path)
{
	std::error_code statusError;
	if (!std::filesystem::exists(path, statusError) && !statusError)
	{
		return;
	}
	std::string text;
	std::string failure;
	if (!Files::TryReadAllText(path, text, failure))
	{
		throw Diagnostics::Error(failure + " Repair or remove this settings file before retrying.");
	}
	std::istringstream input(text);
	std::vector<std::pair<ConsoleVariableBase*, std::string>> assignments;
	bool inTargetSection = false;
	bool foundSection = false;
	for (std::string line; std::getline(input, line);)
	{
		const std::string trimmed = Strings::TrimCopy(line);
		if (trimmed.empty() || trimmed.starts_with(';') || trimmed.starts_with('#'))
		{
			continue;
		}
		if (trimmed.starts_with('[') && trimmed.ends_with(']'))
		{
			inTargetSection = trimmed.substr(1, trimmed.size() - 2) == GetSectionName();
			foundSection = foundSection || inTargetSection;
			continue;
		}
		if (!inTargetSection)
		{
			continue;
		}
		const std::size_t separator = trimmed.find('=');
		if (separator == std::string::npos)
		{
			throw Diagnostics::Error("Malformed settings in " + path.string() + ". Repair or remove this file before retrying.");
		}
		const std::string key = Strings::TrimCopy(std::string_view(trimmed).substr(0, separator));
		if (!IsPersistedName(key))
		{
			continue;
		}
		ConsoleVariableBase* variable = ConsoleVariableRegistry::Get().Find(key);
		if (!variable)
		{
			continue;
		}
		const std::string value = Strings::TrimCopy(std::string_view(trimmed).substr(separator + 1));
		if (!variable->ValidateValueFromString(value, failure))
		{
			throw Diagnostics::Error("Invalid " + key + " in " + path.string() + ": " + failure + ". Repair or remove this file before retrying.");
		}
		assignments.emplace_back(variable, value);
	}
	if (!foundSection || assignments.empty())
	{
		throw Diagnostics::Error("No valid rendering settings in " + path.string() + ". Repair or remove this file before retrying.");
	}
	for (const auto& [variable, value] : assignments)
	{
		variable->TrySetValueFromString(value, failure);
	}
}

void EngineRenderingSettingsPersistence::Write(const EngineRenderingSettingsState& state)
{
	const std::filesystem::path configPath = GetUserConfigPath();
	std::error_code errorCode;
	std::filesystem::create_directories(configPath.parent_path(), errorCode);

	std::vector<std::string> lines;
	{
		std::ifstream input(configPath);
		for (std::string line; std::getline(input, line);)
		{
			lines.push_back(std::move(line));
		}
	}

	std::vector<std::string> sectionLines;
	sectionLines.reserve(GetPersistedNames().size() + 1u);
	sectionLines.emplace_back("[" + std::string(GetSectionName()) + "]");

	const auto appendConfigValue = [&sectionLines]<typename TValue>(std::string_view key, TValue value)
	{
		std::string persistedValue;
		if constexpr (std::is_enum_v<TValue>)
		{
			persistedValue = std::to_string(static_cast<std::underlying_type_t<TValue>>(value));
		}
		else if constexpr (std::is_same_v<TValue, bool>)
		{
			persistedValue = value ? "1" : "0";
		}
		else
		{
			persistedValue = std::to_string(value);
		}
		sectionLines.emplace_back(std::string(key) + "=" + persistedValue);
	};

	appendConfigValue("r.VSync", state.VSync);
	appendConfigValue("r.Vignette", state.Vignette);
	appendConfigValue("r.ChromaticAberration", state.ChromaticAberration);
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	appendConfigValue("r.ExternalCapture.StartupProvider", state.StartupCaptureProvider);
#endif
	appendConfigValue("r.BackBufferFormat", state.BackBufferFormat);
	appendConfigValue("r.PreferHighPerformanceAdapter", state.PreferHighPerformanceAdapter);
	appendConfigValue("r.ToneMapper", state.ToneMapper);
	appendConfigValue("r.Exposure.Mode", state.ExposureMode);
	appendConfigValue("r.Exposure.MeteringMethod", state.ExposureMeteringMethod);
	appendConfigValue("r.OutputColorEncoding", state.OutputColorEncoding);
	appendConfigValue("r.Exposure.Manual", state.ManualExposure);
	appendConfigValue("r.Exposure.Compensation", state.ExposureCompensation);
	appendConfigValue("r.Exposure.TargetLuminance", state.ExposureTargetLuminance);
	appendConfigValue("r.Exposure.Min", state.ExposureMin);
	appendConfigValue("r.Exposure.Max", state.ExposureMax);
	appendConfigValue("r.Exposure.AdaptationSpeedUp", state.ExposureAdaptationSpeedUp);
	appendConfigValue("r.Exposure.AdaptationSpeedDown", state.ExposureAdaptationSpeedDown);
	appendConfigValue("r.MeshAutoBatching", state.MeshAutoBatching);
	appendConfigValue("r.Upscaler.Provider", state.UpscalerProvider);
	appendConfigValue("r.Upscaler.QualityMode", state.UpscalerQualityMode);
	appendConfigValue("r.RayReconstruction.Mode", state.RayReconstructionMode);
	appendConfigValue("r.GBuffer.Algorithm", state.SelectedGBufferAlgorithm);
	appendConfigValue("r.RayTracing.Tlas.Refit", state.RefitTlas);
	appendConfigValue("r.RayTracing.PreferPtlas", state.PtlasActive);
	appendConfigValue("r.RayTracing.Ptlas.PartitionsPerAxis", state.PtlasPartitionsPerAxis);
	appendConfigValue("r.RayTracing.Ptlas.PartitionUpdateMode", state.PtlasPartitionUpdateMode);
	appendConfigValue("r.RayTracing.Ptlas.MarkAllDynamicInPartition", state.PtlasMarkAllDynamicInPartition);
	appendConfigValue("r.RayTracing.Ptlas.ModeChangeDistance", state.PtlasModeChangeDistance);

	std::size_t sectionStart = lines.size();
	std::size_t sectionEnd = lines.size();
	for (std::size_t index = 0; index < lines.size(); ++index)
	{
		const std::string trimmed = Strings::TrimCopy(lines[index]);
		if (!trimmed.starts_with('[') || !trimmed.ends_with(']'))
		{
			continue;
		}
		if (trimmed.size() > 2 && trimmed.substr(1, trimmed.size() - 2) == GetSectionName())
		{
			sectionStart = index;
			sectionEnd = index + 1;
			while (sectionEnd < lines.size())
			{
				const std::string nextTrimmed = Strings::TrimCopy(lines[sectionEnd]);
				if (nextTrimmed.starts_with('[') && nextTrimmed.ends_with(']'))
				{
					break;
				}
				++sectionEnd;
			}
			break;
		}
	}

	if (sectionStart < lines.size())
	{
		lines.erase(lines.begin() + static_cast<std::ptrdiff_t>(sectionStart), lines.begin() + static_cast<std::ptrdiff_t>(sectionEnd));
		lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(sectionStart), sectionLines.begin(), sectionLines.end());
	}
	else
	{
		if (!lines.empty() && !Strings::TrimCopy(lines.back()).empty())
		{
			lines.emplace_back();
		}
		lines.insert(lines.end(), sectionLines.begin(), sectionLines.end());
	}

	std::string output;
	for (const std::string& line : lines)
	{
		output += line;
		output += '\n';
	}
	std::string failure;
	if (!Files::TryWriteAllTextAtomic(configPath, output, failure))
	{
		Files::CleanupTemporaryFile(Files::BuildTemporaryPath(configPath));
		throw Diagnostics::Error(failure);
	}
}

bool EngineRenderingSettingsPersistence::IsPersistedName(std::string_view name) noexcept
{
	for (const std::string_view persistedName : GetPersistedNames())
	{
		if (name == persistedName)
		{
			return true;
		}
	}
	return false;
}
