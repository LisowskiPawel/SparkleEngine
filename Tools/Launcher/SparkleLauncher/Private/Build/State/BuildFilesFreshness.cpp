#include "SparkleLauncher/BuildWorkspaceOperations.h"

#include "Build/State/BuildFreshnessSignature.h"
#include "Build/Toolchains/CMakeGeneratorModel.h"
#include "Build/State/BuildWorkspaceStateFiles.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Json/JsonInputAdmission.h"

#include <nlohmann/json.hpp>
#include "ToolOutputPaths.h"

#include <algorithm>
#include <array>
#include <set>
#include <optional>
#include <string_view>
#include <system_error>

namespace SparkleLauncher
{
	static std::string ToCMakeBool(bool bValue)
	{
		return bValue ? "ON" : "OFF";
	}

	static nlohmann::json ParseFreshnessStamp(std::string_view text)
	{
		try
		{
			Json::InputAdmission admission(text, text.size());
			admission.BeginObject(0);
			std::set<std::string> repeated;
			const std::array<std::string_view, 9> fields{"generator", "platform", "toolset", "qtRoot", "vulkanSdkRoot", "contentPipeline", "shaderCompiler", "nvidiaStreamline", "sourceListHash"};

			auto document = nlohmann::json::parse(
			    text.begin(),
			    text.end(),
			    [&](int depth, nlohmann::json::parse_event_t event, nlohmann::json& value)
			    {
				    using Event = nlohmann::json::parse_event_t;
				    admission.CheckDepth(depth);
				    if (event == Event::key)
				    {
					    const auto& key = value.get_ref<const std::string&>();
					    const bool bConsumed = depth == 1 && std::find(fields.begin(), fields.end(), key) != fields.end();
					    if (!bConsumed)
					    {
						    return false;
					    }
					    if (!admission.TryReadKey(depth, key))
					    {
						    repeated.insert(key);
					    }
				    }
				    return true;
			    });

			if (!document.is_object())
			{
				return nlohmann::json::object();
			}
			for (const auto& key : repeated)
			{
				document.erase(key);
			}
			return document;
		}
		catch (const std::exception&)
		{
			return nlohmann::json::object();
		}
	}

	static nlohmann::json ReadFreshnessStamp(const std::filesystem::path& path)
	{
		std::string text;
		std::string error;
		if (!Files::TryReadAllText(path, text, error))
		{
			return nlohmann::json::object();
		}

		return ParseFreshnessStamp(text);
	}

	BuildFilesFreshnessStatus CheckBuildFilesFreshness(const std::filesystem::path& workspaceRoot, const BuildToolchainStatus& toolchain)
	{
		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(workspaceRoot);
		BuildFilesFreshnessStatus status;
		status.BuildDirectory = workspaceOutputs.BuildRoot;
		status.CachePath = status.BuildDirectory / "CMakeCache.txt";
		status.SolutionPath = GetBuildSolutionPath(workspaceRoot);
		status.StampPath = status.BuildDirectory / "BuildFilesFreshness.json";

		std::error_code errorCode;
		if (!std::filesystem::is_directory(status.BuildDirectory, errorCode))
		{
			status.State = BuildFilesFreshnessState::BuildDirectoryMissing;
			status.Summary = "Build directory is missing.";
			return status;
		}

		if (!std::filesystem::is_regular_file(status.CachePath, errorCode))
		{
			status.State = BuildFilesFreshnessState::CMakeCacheMissing;
			status.Summary = "CMake cache is missing.";
			return status;
		}

		if (CMakeGeneratorProducesSolution(toolchain.Generator) && !std::filesystem::is_regular_file(status.SolutionPath, errorCode))
		{
			status.State = BuildFilesFreshnessState::SolutionMissing;
			status.Summary = "Solution file is missing.";
			return status;
		}

		const std::string cacheGenerator = ReadCMakeCacheValue(status.CachePath, "CMAKE_GENERATOR").value_or(std::string());
		const std::string cachePlatform = ReadCMakeCacheValue(status.CachePath, "CMAKE_GENERATOR_PLATFORM").value_or(std::string());
		const std::string cacheToolset = ReadCMakeCacheValue(status.CachePath, "CMAKE_GENERATOR_TOOLSET").value_or(std::string());
		const std::string cacheQtPrefixPath = ReadCMakeCacheValue(status.CachePath, "CMAKE_PREFIX_PATH").value_or(std::string());
		const std::string selectedPlatform = GetCMakeCachePlatformValue(toolchain);
		const std::string selectedToolset = GetCMakeCacheToolsetValue(toolchain);
		const std::string selectedQtPrefixPath = toolchain.QtRootPath.generic_string();
		const std::string selectedVulkanSdkRoot = toolchain.VulkanSdkRoot.generic_string();
		if (cacheGenerator != toolchain.Generator || cachePlatform != selectedPlatform || cacheToolset != selectedToolset || cacheQtPrefixPath != selectedQtPrefixPath)
		{
			status.State = BuildFilesFreshnessState::GeneratorMismatch;
			status.Summary = "CMake cache generator/platform/toolset/Qt prefix differs from selected launcher toolchain.";
			return status;
		}

		const WorkspaceFeatureSettings featureSettings = GetLauncherWorkspaceFeatureSettings();
		const std::string selectedContentPipeline = ToCMakeBool(featureSettings.bContentPipelineEnabled);
		const std::string selectedShaderCompiler = ToCMakeBool(featureSettings.bShaderCompilerEnabled);
		const std::string selectedNvidiaStreamline = ToCMakeBool(featureSettings.bNvidiaStreamlineEnabled);
		const std::string cacheContentPipeline = ReadCMakeCacheValue(status.CachePath, "SPARKLE_ENABLE_CONTENT_PIPELINE").value_or(std::string());
		const std::string cacheShaderCompiler = ReadCMakeCacheValue(status.CachePath, "SPARKLE_ENABLE_SHADER_COMPILER").value_or(std::string());
		const std::string cacheNvidiaStreamline = ReadCMakeCacheValue(status.CachePath, "SPARKLE_ENABLE_NVIDIA_STREAMLINE").value_or(std::string());
		if (cacheContentPipeline != selectedContentPipeline || cacheShaderCompiler != selectedShaderCompiler || cacheNvidiaStreamline != selectedNvidiaStreamline)
		{
			status.State = BuildFilesFreshnessState::FeatureSetMismatch;
			status.Summary = "CMake cache workspace feature toggles differ from the launcher feature set.";
			return status;
		}

		if (!std::filesystem::is_regular_file(status.StampPath, errorCode))
		{
			status.State = BuildFilesFreshnessState::FreshnessStampMissing;
			status.Summary = "Build-file freshness stamp is missing.";
			return status;
		}

		const auto stamp = ReadFreshnessStamp(status.StampPath);

		const auto matches = [&stamp](const char* key, const std::string& expected)
		{
			const auto entry = stamp.find(key);
			const auto* value = entry == stamp.end() ? nullptr : entry->get_ptr<const std::string*>();
			return value != nullptr ? *value == expected : expected.empty();
		};

		const bool bStampGeneratorMatches = matches("generator", toolchain.Generator);
		const bool bStampPlatformMatches = bStampGeneratorMatches && matches("platform", toolchain.Platform);
		const bool bStampToolsetMatches = bStampPlatformMatches && matches("toolset", toolchain.Toolset);
		const bool bStampQtMatches = bStampToolsetMatches && matches("qtRoot", selectedQtPrefixPath);
		const bool bStampVulkanMatches = bStampQtMatches && matches("vulkanSdkRoot", selectedVulkanSdkRoot);

		if (!bStampVulkanMatches)
		{
			status.State = BuildFilesFreshnessState::FreshnessStampMismatch;
			status.Summary = "Freshness stamp generator/platform/toolset/Qt prefix/Vulkan SDK differs from selected launcher toolchain.";
			return status;
		}

		const bool bStampContentPipelineMatches = matches("contentPipeline", selectedContentPipeline);

		const bool bStampShaderCompilerMatches = bStampContentPipelineMatches && matches("shaderCompiler", selectedShaderCompiler);

		const bool bStampStreamlineMatches = bStampShaderCompilerMatches && matches("nvidiaStreamline", selectedNvidiaStreamline);

		if (!bStampStreamlineMatches)
		{
			status.State = BuildFilesFreshnessState::FeatureSetMismatch;
			status.Summary = "Build-file freshness stamp workspace feature toggles differ from the launcher feature set.";
			return status;
		}

		const std::optional<std::string> sourceListHash = ComputeSourceListHash(workspaceRoot);
		if (!sourceListHash.has_value())
		{
			status.State = BuildFilesFreshnessState::Unsupported;
			status.Summary = "Native source-list hash is unavailable on this platform.";
			return status;
		}

		if (!matches("sourceListHash", *sourceListHash))
		{
			status.State = BuildFilesFreshnessState::SourceListChanged;
			status.Summary = "Source file list changed since build files were generated.";
			return status;
		}

		const std::filesystem::file_time_type stampTime = std::filesystem::last_write_time(status.StampPath, errorCode);
		for (const std::filesystem::path& path : CollectBuildInputPaths(workspaceRoot))
		{
			const std::filesystem::file_time_type inputTime = std::filesystem::last_write_time(path, errorCode);
			if (!errorCode && inputTime > stampTime)
			{
				status.State = BuildFilesFreshnessState::BuildInputChanged;
				status.Summary = "Build input changed: " + MakeFreshnessSignaturePath(workspaceRoot, path);
				return status;
			}
			errorCode.clear();
		}

		status.State = BuildFilesFreshnessState::Current;
		status.bCurrent = true;
		status.Summary = "Build files are current.";
		return status;
	}

	bool UpdateBuildFilesFreshnessStamp(const std::filesystem::path& workspaceRoot, const BuildToolchainStatus& toolchain, std::string& errorMessage)
	{
		const std::optional<std::string> sourceListHash = ComputeSourceListHash(workspaceRoot);
		if (!sourceListHash.has_value())
		{
			errorMessage = "Native source-list hash is unavailable on this platform.";
			return false;
		}

		const WorkspaceFeatureSettings featureSettings = GetLauncherWorkspaceFeatureSettings();

		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(workspaceRoot);
		const std::filesystem::path stampPath = workspaceOutputs.BuildRoot / "BuildFilesFreshness.json";
		std::error_code errorCode;
		std::filesystem::create_directories(stampPath.parent_path(), errorCode);
		if (errorCode)
		{
			errorMessage = "Failed to create build directory for freshness stamp.";
			return false;
		}

		try
		{
			const nlohmann::json stamp{
			    {"generator", toolchain.Generator},
			    {"platform", toolchain.Platform},
			    {"toolset", toolchain.Toolset},
			    {"qtRoot", toolchain.QtRootPath.generic_string()},
			    {"vulkanSdkRoot", toolchain.VulkanSdkRoot.generic_string()},
			    {"contentPipeline", ToCMakeBool(featureSettings.bContentPipelineEnabled)},
			    {"shaderCompiler", ToCMakeBool(featureSettings.bShaderCompilerEnabled)},
			    {"nvidiaStreamline", ToCMakeBool(featureSettings.bNvidiaStreamlineEnabled)},
			    {"sourceListHash", *sourceListHash},
			    {"updatedUtc", BuildUtcTimestamp()}};

			const auto text = stamp.dump(4) + "\n";
			return Files::TryWriteAllTextAtomic(stampPath, text, errorMessage);
		}
		catch (const std::exception&)
		{
			errorMessage = "Failed to encode build freshness stamp.";
			return false;
		}
	}
}
