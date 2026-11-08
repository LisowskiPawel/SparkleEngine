#include "PCH.h"
#include "RenderingSettings/EngineRenderingSettingsPersistence.h"

#include "Core/Public/Console/CVar.h"
#include "Core/Public/Console/CVarRegistry.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"

#include <optional>
#include <sstream>
#include <type_traits>
#include <utility>
#include <vector>

namespace EngineRenderingSettingsPersistence
{
	constexpr std::string_view SettingsSection = "/Script/SparkleRenderer.EngineRenderingSettings";
	using SettingsAssignment = std::pair<ConsoleVariableBase*, std::string>;

	template <typename TVisitor> void VisitPersistedSettings(TVisitor&& visit)
	{
		visit("r.VSync", &EngineRenderingSettingsState::bVSync);
		visit("r.Vignette", &EngineRenderingSettingsState::bVignette);
		visit("r.ChromaticAberration", &EngineRenderingSettingsState::bChromaticAberration);
#if SPARKLE_WITH_EXTERNAL_CAPTURE
		visit("r.ExternalCapture.StartupProvider", &EngineRenderingSettingsState::StartupCaptureProvider);
#endif
		visit("r.BackBufferFormat", &EngineRenderingSettingsState::BackBufferFormat);
		visit("r.PreferHighPerformanceAdapter", &EngineRenderingSettingsState::bPreferHighPerformanceAdapter);
		visit("r.ToneMapper", &EngineRenderingSettingsState::ToneMapper);
		visit("r.Exposure.Mode", &EngineRenderingSettingsState::ExposureMode);
		visit("r.Exposure.MeteringMethod", &EngineRenderingSettingsState::ExposureMeteringMethod);
		visit("r.OutputColorEncoding", &EngineRenderingSettingsState::OutputColorEncoding);
		visit("r.Exposure.Manual", &EngineRenderingSettingsState::ManualExposure);
		visit("r.Exposure.Compensation", &EngineRenderingSettingsState::ExposureCompensation);
		visit("r.Exposure.TargetLuminance", &EngineRenderingSettingsState::ExposureTargetLuminance);
		visit("r.Exposure.Min", &EngineRenderingSettingsState::ExposureMin);
		visit("r.Exposure.Max", &EngineRenderingSettingsState::ExposureMax);
		visit("r.Exposure.AdaptationSpeedUp", &EngineRenderingSettingsState::ExposureAdaptationSpeedUp);
		visit("r.Exposure.AdaptationSpeedDown", &EngineRenderingSettingsState::ExposureAdaptationSpeedDown);
		visit("r.MeshAutoBatching", &EngineRenderingSettingsState::bMeshAutoBatching);
		visit("r.Upscaler.Provider", &EngineRenderingSettingsState::UpscalerProvider);
		visit("r.Upscaler.QualityMode", &EngineRenderingSettingsState::UpscalerQualityMode);
		visit("r.RayReconstruction.Mode", &EngineRenderingSettingsState::RayReconstructionMode);
		visit("r.GBuffer.Algorithm", &EngineRenderingSettingsState::SelectedGBufferAlgorithm);
		visit("r.RayTracing.Tlas.Refit", &EngineRenderingSettingsState::bRefitTlas);
		visit("r.RayTracing.PreferPtlas", &EngineRenderingSettingsState::bPtlasActive);
		visit("r.RayTracing.Ptlas.PartitionsPerAxis", &EngineRenderingSettingsState::PtlasPartitionsPerAxis);
		visit("r.RayTracing.Ptlas.PartitionUpdateMode", &EngineRenderingSettingsState::PtlasPartitionUpdateMode);
		visit("r.RayTracing.Ptlas.MarkAllDynamicInPartition", &EngineRenderingSettingsState::bPtlasMarkAllDynamicInPartition);
		visit("r.RayTracing.Ptlas.ModeChangeDistance", &EngineRenderingSettingsState::PtlasModeChangeDistance);
	}

	static bool IsPersistedName(std::string_view name)
	{
		bool bPersisted = false;
		VisitPersistedSettings([&](std::string_view key, auto) { bPersisted = bPersisted || name == key; });
		return bPersisted;
	}

	static std::filesystem::path GetDefaultConfigPath()
	{
		const auto path = Filesystem::GetWorkspaceRootPath() / "Config" / "DefaultEngine.ini";
		std::error_code error;
		if (!std::filesystem::is_regular_file(path, error) || error)
		{
			throw Diagnostics::Error("Rendering defaults are missing or inaccessible: " + path.string());
		}
		return path;
	}

	static std::filesystem::path GetUserConfigPath()
	{
		return Filesystem::GetProductUserStatePaths().SettingsRoot / "EngineRendering.ini";
	}

	static std::string ReadConfigText(const std::filesystem::path& path)
	{
		std::string text;
		std::string failure;
		if (!Files::TryReadAllText(path, text, failure))
		{
			throw Diagnostics::Error(failure + " Repair or remove this settings file before retrying.");
		}
		return text;
	}

	static bool IsSectionHeader(std::string_view line)
	{
		return line.starts_with('[') && line.ends_with(']');
	}

	static std::optional<SettingsAssignment> ParseAssignment(std::string_view line, const std::filesystem::path& path)
	{
		const std::size_t separator = line.find('=');
		if (separator == std::string_view::npos)
		{
			throw Diagnostics::Error("Malformed settings in " + path.string() + ". Repair or remove this file before retrying.");
		}
		const std::string key = Strings::TrimCopy(line.substr(0, separator));
		if (!IsPersistedName(key))
		{
			return std::nullopt;
		}
		ConsoleVariableBase* variable = ConsoleVariableRegistry::Get().Find(key);
		if (!variable)
		{
			return std::nullopt;
		}
		std::string value = Strings::TrimCopy(line.substr(separator + 1));
		std::string failure;
		if (!variable->ValidateValueFromString(value, failure))
		{
			throw Diagnostics::Error("Invalid " + key + " in " + path.string() + ": " + failure + ". Repair or remove this file before retrying.");
		}
		return SettingsAssignment{variable, std::move(value)};
	}

	static std::vector<SettingsAssignment> ParseAssignments(const std::string& text, const std::filesystem::path& path)
	{
		std::istringstream input(text);
		std::vector<SettingsAssignment> assignments;
		bool bInTargetSection = false;
		bool bFoundSection = false;
		for (std::string line; std::getline(input, line);)
		{
			const std::string trimmed = Strings::TrimCopy(line);
			const bool bComment = trimmed.starts_with(';') || trimmed.starts_with('#');
			if (trimmed.empty() || bComment)
			{
				continue;
			}
			if (IsSectionHeader(trimmed))
			{
				bInTargetSection = trimmed.substr(1, trimmed.size() - 2) == SettingsSection;
				bFoundSection = bFoundSection || bInTargetSection;
				continue;
			}
			if (bInTargetSection)
			{
				if (auto assignment = ParseAssignment(trimmed, path))
				{
					assignments.push_back(std::move(*assignment));
				}
			}
		}
		if (!bFoundSection || assignments.empty())
		{
			throw Diagnostics::Error("No valid rendering settings in " + path.string() + ". Repair or remove this file before retrying.");
		}
		return assignments;
	}

	static void ApplyAssignments(const std::vector<SettingsAssignment>& assignments)
	{
		for (const auto& [variable, value] : assignments)
		{
			std::string failure;
			if (!variable->TrySetValueFromString(value, failure))
			{
				throw Diagnostics::Error(failure);
			}
		}
	}

	static void ApplyFile(const std::filesystem::path& path)
	{
		std::error_code error;
		if (!std::filesystem::exists(path, error) && !error)
		{
			return;
		}
		const std::string text = ReadConfigText(path);
		const std::vector<SettingsAssignment> assignments = ParseAssignments(text, path);
		ApplyAssignments(assignments);
	}

	template <typename TValue> std::string EncodeConfigValue(TValue value)
	{
		if constexpr (std::is_enum_v<TValue>)
		{
			return std::to_string(static_cast<std::underlying_type_t<TValue>>(value));
		}
		else if constexpr (std::is_same_v<TValue, bool>)
		{
			return value ? "1" : "0";
		}
		else
		{
			return std::to_string(value);
		}
	}

	static std::vector<std::string> SerializeSettingsSection(const EngineRenderingSettingsState& state)
	{
		std::vector<std::string> lines;
		lines.emplace_back("[" + std::string(SettingsSection) + "]");
		VisitPersistedSettings([&](std::string_view key, auto member) { lines.emplace_back(std::string(key) + "=" + EncodeConfigValue(state.*member)); });
		return lines;
	}

	static std::vector<std::string> ReadConfigLines(const std::filesystem::path& path)
	{
		std::vector<std::string> lines;
		std::error_code error;
		if (!std::filesystem::exists(path, error) && !error)
		{
			return lines;
		}
		std::istringstream input(ReadConfigText(path));
		for (std::string line; std::getline(input, line);)
		{
			lines.push_back(std::move(line));
		}
		return lines;
	}

	static std::pair<std::size_t, std::size_t> FindSettingsSection(const std::vector<std::string>& lines)
	{
		for (std::size_t begin = 0; begin < lines.size(); ++begin)
		{
			if (Strings::TrimCopy(lines[begin]) != "[" + std::string(SettingsSection) + "]")
			{
				continue;
			}
			std::size_t end = begin + 1;
			while (end < lines.size())
			{
				if (IsSectionHeader(Strings::TrimCopy(lines[end])))
				{
					break;
				}
				++end;
			}
			return {begin, end};
		}
		return {lines.size(), lines.size()};
	}

	static void ReplaceSettingsSection(std::vector<std::string>& lines, const std::vector<std::string>& section)
	{
		const auto [begin, end] = FindSettingsSection(lines);
		if (begin < lines.size())
		{
			lines.erase(lines.begin() + static_cast<std::ptrdiff_t>(begin), lines.begin() + static_cast<std::ptrdiff_t>(end));
			lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(begin), section.begin(), section.end());
			return;
		}
		if (!lines.empty() && !Strings::TrimCopy(lines.back()).empty())
		{
			lines.emplace_back();
		}
		lines.insert(lines.end(), section.begin(), section.end());
	}

	static void PublishConfigLines(const std::filesystem::path& path, const std::vector<std::string>& lines)
	{
		std::string output;
		for (const std::string& line : lines)
		{
			output += line;
			output += '\n';
		}
		std::string failure;
		if (!Files::TryWriteAllTextAtomic(path, output, failure))
		{
			Files::CleanupTemporaryFile(Files::BuildTemporaryPath(path));
			throw Diagnostics::Error(failure);
		}
	}
}

void EngineRenderingSettingsPersistence::Apply()
{
	ApplyFile(GetDefaultConfigPath());
	ApplyFile(GetUserConfigPath());
}

void EngineRenderingSettingsPersistence::Write(const EngineRenderingSettingsState& state)
{
	const std::filesystem::path path = GetUserConfigPath();
	std::vector<std::string> lines = ReadConfigLines(path);
	const std::vector<std::string> section = SerializeSettingsSection(state);
	ReplaceSettingsSection(lines, section);
	PublishConfigLines(path, lines);
}
