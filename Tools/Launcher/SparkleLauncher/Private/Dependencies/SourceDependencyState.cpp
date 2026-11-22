#include "SparkleLauncher/SourceDependencyState.h"

#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "SparkleLauncher/ToolResolver.h"
#include "Core/Public/Process/ChildProcess.h"

#include <cstdint>
#include <sstream>
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

	static std::string JoinPaths(const std::vector<std::string>& paths)
	{
		std::ostringstream stream;
		for (std::size_t index = 0; index < paths.size(); ++index)
		{
			if (index > 0)
			{
				stream << ", ";
			}
			stream << paths[index];
		}
		return stream.str();
	}

	const std::vector<SourceDependencyEntry>& GetSourceDependencies()
	{
		const WorkspaceFeatureSettings features = GetLauncherWorkspaceFeatureSettings();

		static const std::vector<SourceDependencyEntry> dependencies{
		    {"nlohmann_json",
		        "nlohmann/json",
		        "v" SPARKLE_NLOHMANN_JSON_VERSION,
		        "JSON parsing, typed values and serialization.",
		        "nlohmann_json-src",
		        {"CMakeLists.txt", "include/nlohmann/json.hpp", "include/nlohmann/json_fwd.hpp", "LICENSE.MIT"},
		        true,
		        true,
		        SPARKLE_NLOHMANN_JSON_REVISION},
		    {"cli11",
		        "CLI11",
		        "v2.7.2",
		        "Command-line parsing, validation and generated help.",
		        "cli11-src",
		        {"CMakeLists.txt", "include/CLI/CLI.hpp", "include/CLI/App.hpp", "include/CLI/impl/App_inl.hpp", "LICENSE"},
		        true,
		        true},
		    {"imgui", "Dear ImGui", "v1.92.5", "Immediate-mode UI core and Win32 platform backend.", "imgui-src", {"imgui.h", "imgui.cpp", "backends/imgui_impl_win32.cpp"}, true, true},
		    {"spdlog", "spdlog", "v1.14.1", "Repo-wide logging backend.", "spdlog-src", {"include/spdlog/spdlog.h"}, true, true},
		    {"boost-process",
		        "Boost.Process / Asio",
		        "v" SPARKLE_BOOST_VERSION,
		        "Native child creation, environment, output and completion.",
		        "boost-process-src",
		        {SPARKLE_BOOST_REQUIRED_FILES},
		        true,
		        true,
		        SPARKLE_BOOST_REVISION},
		    {"ozz_animation",
		        "ozz-animation",
		        "v0.17.0",
		        "Animation preview construction, sampling and joint hierarchy evaluation.",
		        "ozz_animation-src",
		        {"CMakeLists.txt",
		            "LICENSE.md",
		            "include/ozz/animation/runtime/skeleton.h",
		            "include/ozz/animation/runtime/animation.h",
		            "include/ozz/animation/runtime/sampling_job.h",
		            "include/ozz/animation/offline/skeleton_builder.h",
		            "include/ozz/animation/offline/animation_builder.h",
		            "include/ozz/base/memory/allocator.h"},
		        false,
		        true,
		        SPARKLE_OZZ_ANIMATION_REVISION},
		    {"editor-icons",
		        "Font Awesome Free Solid",
		        "v6.7.1",
		        "Launcher/editor icon font asset and license.",
		        "editor-icons",
		        {"fontawesome-6.7.1/fa-solid-900.ttf", "fontawesome-6.7.1/LICENSE.txt"},
		        true,
		        true},
		    {"cgltf", "cgltf", "v1.15", "Single-header glTF 2.0 parser for source scene imports.", "cgltf-src", {"cgltf.h"}, false, features.bContentPipelineEnabled},
		    {"stb", "stb", "git 2c980bb", "PSD/PIC/PNM/TGA source decoding.", "stb-src", {"stb_image.h", "LICENSE"}, false, features.bContentPipelineEnabled},
		    {"tinyexr", "tinyexr", "v1.0.7", "Header-only OpenEXR image loading support.", "tinyexr-src", {"tinyexr.h"}, false, features.bContentPipelineEnabled},
		    {"zlib", "zlib", "v1.3.1", "Compression backend used by Assimp and EXR decoding.", "zlib-src", {"zlib.h", "CMakeLists.txt"}, false, features.bContentPipelineEnabled},
		    {"assimp", "Assimp", "v5.4.3", "FBX and DCC scene import support.", "assimp-src", {"include/assimp/Importer.hpp", "CMakeLists.txt"}, false, features.bContentPipelineEnabled},
		    {"directxtex",
		        "DirectXTex",
		        "may2026 / git 4feb3e1",
		        "CPU texture decoding, conversion, mip generation and BC encoding.",
		        "directxtex-src",
		        {"CMakeLists.txt", "DirectXTex/DirectXTex.h", "DirectXTex/DirectXTexDDS.cpp", "DirectXTex/DirectXTexMipmaps.cpp", "DirectXTex/DirectXTexCompress.cpp", "LICENSE"},
		        false,
		        features.bContentPipelineEnabled,
		        SPARKLE_DIRECTXTEX_REVISION},
		    {"ktx",
		        "KTX-Software",
		        "v4.3.2",
		        "Native KTX1/KTX2 source containers.",
		        "ktx-src",
		        {"include/ktx.h",
		            "CMakeLists.txt",
		            "lib/texture.c",
		            "lib/texture1.c",
		            "lib/texture2.c",
		            "lib/basis_transcode.cpp",
		            "lib/miniz_wrapper.cpp",
		            "lib/basisu/encoder/basisu_miniz.h",
		            "lib/basisu/zstd/zstd.c",
		            "cmake/version.cmake",
		            "include/KHR/khr_df.h",
		            "LICENSE.md",
		            "NOTICE.md",
		            "LICENSES/Apache-2.0.txt",
		            "LICENSES/BSD-1-Clause.txt",
		            "LICENSES/LicenseRef-ETCSLA.txt",
		            "lib/basisu/zstd/LICENSE"},
		        false,
		        features.bContentPipelineEnabled,
		        SPARKLE_KTX_REVISION},
		    {"shadermake",
		        "ShaderMake",
		        "git " SPARKLE_SHADERMAKE_REVISION " + compiler-contract",
		        "Offline DXC/Slang batch execution for the shader cooker.",
		        "shadermake-src",
		        {"ShaderMake/CMakeLists.txt",
		            "ShaderMake/ShaderMake.cpp",
		            "ShaderMake/ShaderBlob.cpp",
		            "ShaderMake/ShaderBlob.h",
		            "ShaderMake/argparse.c",
		            "ShaderMake/argparse.h",
		            "LICENSE.txt",
		            "ThirdPartyLicenses.txt"},
		        false,
		        features.bShaderCompilerEnabled,
		        SPARKLE_SHADERMAKE_REVISION},
		    {"spirv-reflect",
		        "SPIRV-Reflect",
		        "vulkan-sdk-1.3.290.0",
		        "SPIR-V reflection for offline shader compiler backends.",
		        "spirv_reflect-src",
		        {"spirv_reflect.h", "spirv_reflect.c"},
		        false,
		        features.bShaderCompilerEnabled},
		    {"nvidia-nvapi",
		        "NVIDIA NVAPI SDK",
		        "git 9b181ea",
		        "Headers and import library used by D3D12 NVAPI integration and PTLAS capability support.",
		        "sparkle_nvapi-src",
		        {"nvapi.h", "amd64/nvapi64.lib"},
		        false,
		        features.bNvidiaStreamlineEnabled},
		    {"nvidia-streamline",
		        "NVIDIA Streamline SDK",
		        "v2.11.1",
		        "Headers, import library, Streamline plugins, and DLSS runtime redistributables.",
		        "streamline-sdk-src",
		        {"include/sl.h",
		            "lib/x64/sl.interposer.lib",
		            "bin/x64/sl.interposer.dll",
		            "bin/x64/sl.common.dll",
		            "bin/x64/sl.dlss.dll",
		            "bin/x64/sl.dlss_d.dll",
		            "bin/x64/sl.pcl.dll",
		            "bin/x64/sl.reflex.dll",
		            "bin/x64/NvLowLatencyVk.dll",
		            "bin/x64/nvngx_dlss.dll",
		            "bin/x64/nvngx_dlssd.dll"},
		        false,
		        features.bNvidiaStreamlineEnabled},
		};

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

	SourceDependencyValidation ValidateSourceDependency(const SourceDependencyEntry& dependency, const std::filesystem::path& dependencyCacheRoot)
	{
		SourceDependencyValidation validation;
		validation.CachePath = dependencyCacheRoot / dependency.CacheDirectoryName;
		for (const std::string& relativePath : dependency.RequiredRelativePaths)
		{
			if (!PathHasContent(validation.CachePath / relativePath))
			{
				validation.MissingRelativePaths.push_back(relativePath);
			}
		}

		if (!dependency.ExpectedGitRevision.empty() && validation.MissingRelativePaths.empty())
		{
			const ToolResolveResult git = ResolveKnownTool(KnownTool::Git);
			Process::ChildProcessRequest request;
			request.ExecutablePath = git.Path;
			request.WorkingDirectory = validation.CachePath;
			request.Environment = {{"GIT_TERMINAL_PROMPT", "0"}, {"GIT_NO_LAZY_FETCH", "1"}};
			request.Arguments = {"rev-parse", "HEAD"};
			const auto revision = Process::RunChildProcess(request);
			request.Arguments = {"status", "--porcelain", "--untracked-files=all", "--ignore-submodules=none"};
			const auto state = Process::RunChildProcess(request);
			const std::string expectedLine = dependency.ExpectedGitRevision + "\n";
			const std::string expectedWindowsLine = dependency.ExpectedGitRevision + "\r\n";
			const bool bExpectedRevision = revision.CapturedOutput == expectedLine || revision.CapturedOutput == expectedWindowsLine;
			const bool bCommandsSucceeded = revision.Succeeded() && state.Succeeded();
			const bool bClean = state.CapturedOutput.empty() && bExpectedRevision;
			if (!bCommandsSucceeded || !bClean)
			{
				validation.IdentityFailure = "Cache must be clean at " + dependency.ExpectedGitRevision + ". Preserve edits, then explicitly Clean and Sync this dependency.";
			}
		}
		validation.bReady = validation.MissingRelativePaths.empty() && validation.IdentityFailure.empty();
		return validation;
	}

	std::vector<std::filesystem::path> GetSourceDependencyCachePaths(const SourceDependencyEntry& dependency, const std::filesystem::path& dependencyCacheRoot)
	{
		std::vector<std::filesystem::path> paths{dependencyCacheRoot / dependency.CacheDirectoryName};
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
			const SourceDependencyValidation validation = ValidateSourceDependency(dependency, dependencyCacheRoot);
			if (validation.bReady)
			{
				++status.ReadyDependencyCount;
				continue;
			}

			status.bAllEnabledDependenciesReady = false;
			const std::string reason = validation.IdentityFailure.empty() ? "Missing: " + JoinPaths(validation.MissingRelativePaths) + "." : validation.IdentityFailure;
			status.ReadinessMessages.push_back(dependency.Label + " cache is not ready. " + reason);
		}

		if (status.EnabledDependencyCount == 0)
		{
			status.bAllEnabledDependenciesReady = true;
		}

		return status;
	}
}
