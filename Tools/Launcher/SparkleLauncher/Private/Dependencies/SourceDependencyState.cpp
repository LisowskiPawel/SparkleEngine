#include "SparkleLauncher/SourceDependencyState.h"

#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "Dependencies/SourceDependencyCacheAccess.h"
#include "Dependencies/SourceDependencyFacts.h"
#include "Core/Public/Strings/StringUtils.h"

#include <cstdint>
#include <algorithm>
#include <system_error>

namespace SparkleLauncher
{
	static bool PathHasContent(const std::filesystem::path& path)
	{
		std::error_code errorCode;
		if (!std::filesystem::is_regular_file(path, errorCode) || errorCode)
		{
			return false;
		}

		const std::uintmax_t size = std::filesystem::file_size(path, errorCode);
		return !errorCode && size > 0;
	}

	struct SourceDependencyPresentation final
	{
		std::string_view Id;
		std::string_view Purpose;
		bool bRequired;
		bool bEnabled;
	};

	const std::vector<SourceDependencyEntry>& GetSourceDependencies()
	{
		static const std::vector<SourceDependencyEntry> dependencies = []
		{
			const WorkspaceFeatureSettings features = GetLauncherWorkspaceFeatureSettings();
			const SourceDependencyPresentation presentation[]{
			    {"nlohmann_json", "JSON parsing, typed values and serialization.", true, true},
			    {"cli11", "Command-line parsing, validation and generated help.", true, true},
			    {"sdl3", "Native window, event, input and cursor mechanics.", false, true},
			    {"imgui", "Immediate-mode UI with SDL3 and native rendering backends.", false, true},
			    {"spdlog", "Repo-wide logging backend.", true, true},
			    {"boost-process", "Native child creation, environment, output and completion.", true, true},
			    {"enkits", "Native task queues, workers, dependencies and completion; maintained runtime fork.", true, true},
			    {"ozz_animation", "Animation construction, sampling and joint hierarchy evaluation.", false, true},
			    {"editor-icons", "Launcher/editor icon font asset and license.", true, true},
			    {"cgltf", "glTF 2.0 source scene parsing.", false, features.bContentPipelineEnabled},
			    {"mikktspace", "Canonical tangent generation for source scene imports.", false, features.bContentPipelineEnabled},
			    {"stb", "PSD/PIC/PNM/TGA source decoding.", false, features.bContentPipelineEnabled},
			    {"tinyexr", "EXR source loading and Editor image export.", false, true},
			    {"zlib", "Compression used by Assimp and EXR processing.", false, true},
			    {"assimp", "FBX and PLY source scene imports.", false, features.bContentPipelineEnabled},
			    {"directxtex", "CPU texture decoding, conversion, mip generation and BC encoding.", false, features.bContentPipelineEnabled},
			    {"shadermake", "Offline DXC/Slang batch execution for the shader cooker.", false, features.bShaderCompilerEnabled},
			    {"spirv-reflect", "SPIR-V reflection for offline shader compilation.", false, features.bShaderCompilerEnabled},
			    {"nvidia-nvapi", "D3D12 NVAPI and PTLAS capability support.", false, SPARKLE_SOURCE_NVAPI_ENABLED != 0},
			    {"nvidia-streamline", "Streamline plugins and DLSS runtime redistributables.", false, features.bNvidiaStreamlineEnabled},
			    {"winpixeventruntime", "Official PIX event and capture integration inputs.", false, SPARKLE_SOURCE_PIX_ENABLED != 0},
			    {"vma", "Vulkan device memory allocation implementation.", false, SPARKLE_SOURCE_VULKAN_ENABLED != 0},
			    {"d3d12ma", "D3D12 device memory allocation implementation.", false, SPARKLE_SOURCE_D3D12_ENABLED != 0},
			};
			auto entries = BuildSourceDependencyEntries();
			for (auto& entry : entries)
			{
				const auto description = std::find_if(std::begin(presentation), std::end(presentation), [&entry](const auto& candidate) { return candidate.Id == entry.Id; });
				if (description != std::end(presentation))
				{
					entry.Purpose = description->Purpose;
					entry.bRequired = description->bRequired;
					entry.bEnabled = description->bEnabled;
				}
			}
			return entries;
		}();
		return dependencies;
	}

	const SourceDependencyEntry* FindSourceDependency(std::string_view id)
	{
		for (const SourceDependencyEntry& dependency : GetSourceDependencies())
		{
			if (dependency.Id == id)
			{
				return &dependency;
			}
		}
		return nullptr;
	}

	SourceDependencyValidation InspectSourceDependencyAvailability(const SourceDependencyEntry& dependency, const std::filesystem::path& dependencyCacheRoot)
	{
		SourceDependencyValidation validation;
		validation.CachePath = dependencyCacheRoot / dependency.CacheDirectoryName;
		const SourceDependencyCacheAccess cacheAccess(dependencyCacheRoot.parent_path().parent_path(), SourceDependencyCacheAccessMode::Read);
		if (!cacheAccess.IsAcquired())
		{
			validation.FailureReason = cacheAccess.GetErrorMessage();
			return validation;
		}
		for (const std::string& relativePath : dependency.RequiredRelativePaths)
		{
			if (!PathHasContent(validation.CachePath / relativePath))
			{
				validation.MissingRelativePaths.push_back(relativePath);
			}
		}

		validation.bReady = validation.MissingRelativePaths.empty() && validation.FailureReason.empty();
		return validation;
	}

	std::vector<std::filesystem::path> GetSourceDependencyCachePaths(const SourceDependencyEntry& dependency, const std::filesystem::path& dependencyCacheRoot)
	{
		std::vector<std::filesystem::path> paths{dependencyCacheRoot / dependency.CacheDirectoryName};
		for (const std::string& relativePath : dependency.AdditionalCacheFiles)
		{
			paths.push_back(dependencyCacheRoot / relativePath);
			paths.push_back(dependencyCacheRoot / (relativePath + ".download"));
		}
		std::error_code errorCode;
		const std::string stagingPrefix = dependency.CacheDirectoryName + ".incoming-";
		for (std::filesystem::directory_iterator iterator(dependencyCacheRoot, errorCode), end; iterator != end && !errorCode; iterator.increment(errorCode))
		{
			const std::string name = iterator->path().filename().string();
			if (name.starts_with(stagingPrefix) && name.size() == stagingPrefix.size() + 16)
			{
				const std::string_view suffix = std::string_view(name).substr(stagingPrefix.size());
				const bool bOwnedStaging = std::all_of(suffix.begin(), suffix.end(), [](char value) { return (value >= '0' && value <= '9') || (value >= 'a' && value <= 'f'); });
				if (bOwnedStaging)
				{
					paths.push_back(iterator->path());
				}
			}
		}
		constexpr std::string_view sourceSuffix = "-src";
		if (dependency.CacheDirectoryName.ends_with(sourceSuffix))
		{
			const std::string stem = dependency.CacheDirectoryName.substr(0, dependency.CacheDirectoryName.size() - sourceSuffix.size());
			paths.push_back(dependencyCacheRoot / (stem + "-build"));
			paths.push_back(dependencyCacheRoot / (stem + "-subbuild"));
		}
		return paths;
	}

	SourceDependencyInventoryStatus InspectSourceDependencyCache(const std::filesystem::path& dependencyCacheRoot)
	{
		SourceDependencyInventoryStatus status;
		status.CacheRoot = dependencyCacheRoot;

		for (const SourceDependencyEntry& dependency : GetSourceDependencies())
		{
			if (!dependency.bEnabled)
			{
				continue;
			}

			++status.EnabledDependencyCount;
			const SourceDependencyValidation validation = InspectSourceDependencyAvailability(dependency, dependencyCacheRoot);
			if (validation.bReady)
			{
				++status.ReadyDependencyCount;
				continue;
			}

			status.bAllEnabledDependenciesReady = false;
			const std::string reason = validation.FailureReason.empty() ? "Missing: " + Strings::Join(validation.MissingRelativePaths, ", ") + "." : validation.FailureReason;
			status.ReadinessMessages.push_back(dependency.Label + " cache is not ready. " + reason);
		}

		if (status.EnabledDependencyCount == 0)
		{
			status.bAllEnabledDependenciesReady = true;
		}

		return status;
	}
}
