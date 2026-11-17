#include "PCH.h"

#include "Cooking/Compilation/ShaderCompileFailureReplay.h"

#include "Cooking/Compilation/ShaderCompileJob.h"

#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Formatting/HexFormat.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <span>
#include <string>
#include <vector>

namespace ShaderCompileFailureReplay
{
	static constexpr std::size_t kMaximumFailureDiagnosticBytes = 64u * 1024u;
	static constexpr std::size_t kMaximumFailureSourceBytes = 1024u * 1024u;
	static constexpr std::size_t kMaximumFailureListEntries = 1024u;
	static constexpr std::size_t kMaximumFailureListEntryBytes = 1024u;

	static std::string BoundText(std::string_view text, std::size_t maximumBytes)
	{
		if (text.size() <= maximumBytes)
		{
			return std::string(text);
		}

		constexpr std::string_view truncationMarker = "\n<truncated>";
		if (maximumBytes <= truncationMarker.size())
		{
			return std::string(truncationMarker.substr(0, maximumBytes));
		}
		std::size_t retainedBytes = maximumBytes - truncationMarker.size();
		while (retainedBytes > 0 && (static_cast<unsigned char>(text[retainedBytes]) & 0xc0u) == 0x80u)
		{
			--retainedBytes;
		}
		std::string bounded(text.substr(0, retainedBytes));
		bounded += truncationMarker;
		return bounded;
	}

	template <typename T, typename Projection = std::identity> static std::vector<std::string> BoundStrings(std::span<const T> values, Projection project = {})
	{
		const std::size_t retainedCount = std::min(values.size(), kMaximumFailureListEntries);
		std::vector<std::string> bounded;
		bounded.reserve(retainedCount + static_cast<std::size_t>(values.size() > retainedCount));

		for (std::size_t index = 0; index < retainedCount; ++index)
		{
			const auto& text = project(values[index]);
			bounded.push_back(BoundText(text, kMaximumFailureListEntryBytes));
		}
		if (values.size() > retainedCount)
		{
			bounded.emplace_back("<truncated>");
		}
		return bounded;
	}

	void Write(const std::filesystem::path& cookedShaderRoot, const ShaderCompileJob& job, std::string_view diagnostic) noexcept
	{
		try
		{
			const std::vector<std::string> replayDependencies = BoundStrings(std::span(job.VirtualDependencies));
			const std::vector<std::string> replayDefines = BoundStrings(std::span(job.Request.Defines));

			const std::vector<std::string> replayRemaps = BoundStrings(
			    std::span(job.Request.DescriptorBindingRemaps),
			    [](const ShaderDescriptorBindingRemap& remap) { return std::format("{}:{}:{}", remap.Name, remap.Set, remap.Binding); });

			nlohmann::json document{
			    {"shaderType", job.Request.ShaderTypeName},
			    {"shaderTypeId", Formatting::FormatHexUInt64(job.Request.ShaderType)},
			    {"virtualSource", job.Request.VirtualSourcePath},
			    {"entryPoint", job.Request.EntryPoint},
			    {"stage", GetShaderStagePrefix(job.Request.Stage)},
			    {"target", GetShaderTargetName(job.Request.Target)},
			    {"backend", job.BackendName},
			    {"backendVersion", job.BackendVersion},
			    {"profile", job.TargetProfile},
			    {"unitKind", static_cast<std::uint64_t>(job.Request.UnitKind)},
			    {"requiredFeatures", static_cast<std::uint64_t>(job.Request.RequiredFeatures)},
			    {"enableDebugInfo", job.Request.bEnableDebugInfo},
			    {"enableOptimizations", job.Request.bEnableOptimizations},
			    {"treatWarningsAsErrors", job.Request.bTreatWarningsAsErrors},
			    {"stripDebugInfo", job.Request.bStripDebugInfo},
			    {"sourceContentHash", Formatting::FormatHexUInt64(job.SourceContentHash)},
			    {"dependencyClosureHash", Formatting::FormatHexUInt64(job.DependencyClosureHash)},
			    {"requestHash", Formatting::FormatHexUInt64(job.RequestHash)},
			    {"compileInputHash", Formatting::FormatHexUInt64(job.InputHash)},
			    {"virtualDependencies", replayDependencies},
			    {"defines", replayDefines},
			    {"descriptorBindingRemaps", replayRemaps},
			    {"diagnostic", BoundText(diagnostic, kMaximumFailureDiagnosticBytes)},
			    {"preprocessedSource", BoundText(job.Request.SourceCode, kMaximumFailureSourceBytes)}};

			std::string fileError;
			Files::TryWriteAllTextAtomic(cookedShaderRoot / "Diagnostics" / "LastShaderCompileFailure.json", document.dump(), fileError);
		}
		catch (...)
		{
			return;
		}
	}
}
