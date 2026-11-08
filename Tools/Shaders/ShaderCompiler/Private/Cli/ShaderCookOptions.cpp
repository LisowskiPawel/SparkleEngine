#include "Cli/ShaderCookOptions.h"

#include "Cooking/ShaderCookSettings.h"
#include "Core/Public/Strings/StringUtils.h"
#include "RHI/Public/Shaders/ShaderTarget.h"

#include <CLI/CLI.hpp>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

static std::string TransformShaderTarget(std::string& value)
{
	for (std::uint16_t candidate = static_cast<std::uint16_t>(ShaderTarget::DxilSm60); candidate <= static_cast<std::uint16_t>(ShaderTarget::SpirV16); ++candidate)
	{
		const auto target = static_cast<ShaderTarget>(candidate);
		if (value == GetShaderTargetName(target))
		{
			value = std::to_string(candidate);
			return {};
		}
	}
	return "Unknown shader target '" + value + "'";
}

static void SelectTarget(ShaderCookSettings& settings, const CLI::App& cook, ShaderTarget target)
{
	const auto& order = cook.parse_order();
	const CLI::Option* targetOption = order.back();
	const bool bFirstExplicitTarget = std::ranges::count(order, targetOption) == 1;
	if (bFirstExplicitTarget)
	{
		settings.targets.clear();
	}

	if (std::ranges::find(settings.targets, target) == settings.targets.end())
	{
		settings.targets.push_back(target);
	}
}

static void AppendAnalysisPasses(ShaderCookSettings& settings, const std::string& value)
{
	for (const std::string_view pass : Strings::Split(value, ',', false))
	{
		settings.analysisPasses.emplace_back(pass);
	}
}

static std::string ValidateChangedPath(std::string& value)
{
	return value.starts_with('/') ? std::string{} : "requires a canonical virtual shader path beginning with '/'";
}

static std::string ValidateCompileCountSyntax(std::string& value)
{
	return value.find_first_not_of("0123456789") == std::string::npos ? std::string{} : "requires an unsigned decimal count";
}

void ValidateShaderCookSelection(const ShaderCookSettings& settings)
{
	if (!settings.shaderId.empty() && !settings.changedVirtualPaths.empty())
	{
		throw CLI::ValidationError("Use one selection: --shader-id or one or more --changed paths");
	}
}

void ConfigureShaderCookOptions(CLI::App& cook, ShaderCookSettings& settings)
{
	cook.set_help_flag("-h,--help", "Show cook options and exit.");
	cook.option_defaults()->take_last();
	cook.option_defaults()->group("Selection");
	cook.add_option("--shader-id", settings.shaderId, "Cook one registered shader entry by shader ID; excludes --changed.");

	cook.add_option_function<std::string>(
	        "--changed",
	        [&settings](const std::string& path) { settings.changedVirtualPaths.push_back(path); },
	        "Cook shader types affected by a changed virtual source; repeat for each path.")
	    ->check(CLI::Validator(ValidateChangedPath, "VIRTUAL-PATH"));

	cook.option_defaults()->group("Cook options");
	std::vector<std::string_view> defaultTargets;
	for (const ShaderTarget target : settings.targets)
	{
		defaultTargets.emplace_back(GetShaderTargetName(target));
	}
	const std::string targetDefaults = Strings::Join(defaultTargets, ",");

	cook.add_option_function<ShaderTarget>(
	        "--target",
	        std::bind_front(SelectTarget, std::ref(settings), std::cref(cook)),
	        "Codegen target; repeat to select targets. Explicit targets replace defaults and are deduplicated.")
	    ->transform(CLI::Validator(TransformShaderTarget, "TARGET"))
	    ->default_str(targetDefaults);

	cook.add_option("--backend", settings.backendName, "Compiler backend name, or auto.")->capture_default_str();

	cook.add_option_function<std::string>(
	    "--debug-artifacts",
	    [&settings](const std::string& directory) { settings.debugArtifactDirectory = std::filesystem::path(directory); },
	    "Export exact code, full debug data and private source to a fresh cook directory.");

	cook.add_option_function<std::string>(
	    "--analysis",
	    std::bind_front(AppendAnalysisPasses, std::ref(settings)),
	    "Optional analysis passes, such as cooked-shader-stats; repeat or separate with commas.");

	cook.add_option("--parallel-compiles", settings.maximumParallelCompiles, "Bound concurrent compiler sessions; use 1 as the serial oracle.")
	    ->check(CLI::Validator(ValidateCompileCountSyntax, "UINT"))
	    ->check(CLI::Range(1, 8))
	    ->capture_default_str();

	cook.add_flag("--debug-info", settings.bEnableDebugInfo, "Export full debug data under the workspace shader symbols root. Optimization is unchanged.");
	cook.add_flag("!--disable-optimizations", settings.bEnableOptimizations, "Build shaders without backend optimization passes.");

	cook.add_option("--warnings-as-errors", settings.bTreatWarningsAsErrors, "Treat shader warnings as hard errors; on/true or off/false.")
	    ->check(CLI::IsMember({"on", "true", "off", "false"}))
	    ->default_str(settings.bTreatWarningsAsErrors ? "on" : "off");

	cook.add_option("--strip-debug", settings.bStripDebugInfo, "Strip embedded debug info from final runtime binaries when supported; on/true or off/false.")
	    ->check(CLI::IsMember({"on", "true", "off", "false"}))
	    ->default_str(settings.bStripDebugInfo ? "on" : "off");

	for (CLI::Option* option : cook.get_options())
	{
		option->trigger_on_parse();
	}
}
