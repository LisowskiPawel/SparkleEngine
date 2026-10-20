#include "PCH.h"

#include "Cooking/ShaderCompileFailureReplay.h"

#include "Cooking/ShaderCompileJob.h"

#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Json/JsonWriter.h"

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
		std::string bounded(text.substr(0, maximumBytes - truncationMarker.size()));
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

			Json::ObjectWriter writer;
			writer.WriteString("shaderType", job.Request.ShaderTypeName);
			writer.WriteHexUInt64("shaderTypeId", job.Request.ShaderType);
			writer.WriteString("virtualSource", job.Request.VirtualSourcePath);
			writer.WriteString("entryPoint", job.Request.EntryPoint);
			writer.WriteString("stage", GetShaderStagePrefix(job.Request.Stage));
			writer.WriteString("target", GetShaderTargetName(job.Request.Target));
			writer.WriteString("backend", job.BackendName);
			writer.WriteUInt64("backendVersion", job.BackendVersion);
			writer.WriteString("profile", job.TargetProfile);
			writer.WriteUInt64("unitKind", static_cast<std::uint64_t>(job.Request.UnitKind));
			writer.WriteUInt64("requiredFeatures", static_cast<std::uint64_t>(job.Request.RequiredFeatures));
			writer.WriteRaw("enableDebugInfo", job.Request.EnableDebugInfo ? "true" : "false");
			writer.WriteRaw("enableOptimizations", job.Request.EnableOptimizations ? "true" : "false");
			writer.WriteRaw("treatWarningsAsErrors", job.Request.TreatWarningsAsErrors ? "true" : "false");
			writer.WriteRaw("stripDebugInfo", job.Request.StripDebugInfo ? "true" : "false");
			writer.WriteHexUInt64("sourceContentHash", job.SourceContentHash);
			writer.WriteHexUInt64("dependencyClosureHash", job.DependencyClosureHash);
			writer.WriteHexUInt64("requestHash", job.RequestHash);
			writer.WriteHexUInt64("compileInputHash", job.InputHash);
			writer.WriteRaw("virtualDependencies", Json::WriteStringArray(replayDependencies));
			writer.WriteRaw("defines", Json::WriteStringArray(replayDefines));
			writer.WriteRaw("descriptorBindingRemaps", Json::WriteStringArray(replayRemaps));
			writer.WriteString("diagnostic", BoundText(diagnostic, kMaximumFailureDiagnosticBytes));
			writer.WriteString("preprocessedSource", BoundText(job.Request.SourceCode, kMaximumFailureSourceBytes));

			std::string fileError;
			Files::TryWriteAllTextAtomic(cookedShaderRoot / "Diagnostics" / "LastShaderCompileFailure.json", writer.Finish(), fileError);
		}
		catch (...)
		{
			return;
		}
	}
}
