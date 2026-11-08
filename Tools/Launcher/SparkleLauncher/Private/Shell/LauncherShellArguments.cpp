#include "Shell/LauncherShellArguments.h"

#include "SparkleLauncher/BuildProfileCatalog.h"

#include <CLI/CLI.hpp>

#include <algorithm>
#include <functional>
#include <ostream>
#include <string_view>
#include <vector>

namespace SparkleLauncher
{
	static constexpr std::string_view UnrecognizedArgumentPrefix = "---";

	static std::vector<std::string> GetBuildProfileNames(BuildProfileTarget target)
	{
		std::vector<std::string> names;
		for (const BuildProfile& profile : GetBuildProfileCatalog())
		{
			if (profile.Target == target)
			{
				names.push_back(profile.Name);
			}
		}
		return names;
	}

	static void ParseWorkspaceIde(LauncherShellArguments& arguments, const std::string& value)
	{
		const bool bParsed = TryParseWorkspaceIde(value, arguments.WorkspaceIdePreference);
		if (!bParsed)
		{
			throw CLI::ValidationError("--ide", "unsupported IDE '" + value + "'");
		}
	}

	static void ParseWorkspaceCompiler(LauncherShellArguments& arguments, const std::string& value)
	{
		const bool bParsed = TryParseWorkspaceCompiler(value, arguments.WorkspaceCompilerPreference);
		if (!bParsed)
		{
			throw CLI::ValidationError("--compiler", "unsupported compiler '" + value + "'");
		}
	}

	static void ParseCleanScope(LauncherShellArguments& arguments, const std::string& value)
	{
		const bool bParsed = TryParseCleanScope(value, arguments.RequestedCleanScope);
		if (!bParsed)
		{
			throw CLI::ValidationError("--clean-scope", "unsupported clean scope '" + value + "'");
		}
	}

	static std::string ValidateOperationId(const std::string& value)
	{
		return value.starts_with('-') ? "requires an operation id" : "";
	}

	static void SelectLaunchProduct(LauncherShellArguments& arguments, const std::string& value)
	{
		arguments.Product = value == "editor" ? LevelRunMode::Editor : LevelRunMode::Game;
	}

	static void SelectGpuCrashMode(LauncherShellArguments& arguments, const std::string& value)
	{
		arguments.GpuCrashMode = value == "off" ? RhiGpuCrashMode::Off : RhiGpuCrashMode::Aftermath;
	}

	static std::string ValidateSingleUseOption(const CLI::App& app, const CLI::Option* option, std::string&)
	{
		const auto occurrences = std::ranges::count(app.parse_order(), option);
		return occurrences > 1 ? "duplicate GPU crash option" : "";
	}

	static void ConfigureLauncherOptions(CLI::App& app, LauncherShellArguments& arguments)
	{
		app.set_help_flag();
		app.allow_windows_style_options(false);
		app.allow_extras()->prefix_command();
		app.add_flag("--help,-h", arguments.bShowHelp, "Show help (also /?).");

		app.option_defaults()->group("Workspace");
		app.add_option("--root", arguments.StartPath, "Workspace root.")->type_name("PATH");

		app.add_option("--editor-profile", arguments.EditorProfile, "Editor build profile.")->check(CLI::IsMember(GetBuildProfileNames(BuildProfileTarget::Editor)))->capture_default_str();

		app.add_option("--game-profile", arguments.GameProfile, "Game build profile.")->check(CLI::IsMember(GetBuildProfileNames(BuildProfileTarget::Game)))->capture_default_str();

		app.add_option_function<std::string>("--ide", std::bind_front(ParseWorkspaceIde, std::ref(arguments)), "Workspace IDE: visual-studio or rider.");

		app.add_option_function<std::string>("--compiler", std::bind_front(ParseWorkspaceCompiler, std::ref(arguments)), "Workspace compiler: msvc or clang-cl.");

		app.option_defaults()->group("Scene selection");
		app.add_option("--level", arguments.LevelId, "Catalog level for levels.sync or levels.run.")->type_name("ID");

		app.add_option_function<std::string>("--product", std::bind_front(SelectLaunchProduct, std::ref(arguments)), "Application to launch.")->check(CLI::IsMember({"editor", "game"}));

		app.add_option("--graphics-api", arguments.GraphicsApi, "Graphics API for scene launch: d3d12 or vulkan.")->capture_default_str();

		app.option_defaults()->group("Operations");

		app.add_option("--dry-run", arguments.DryRunOperationId, "Preview an operation; defaults to workspace.sync-code when its ID is omitted.")
		    ->expected(0, 1)
		    ->default_str("workspace.sync-code")
		    ->type_name("ID");

		app.add_option("--run", arguments.RunOperationId, "Execute an operation.")->check(CLI::Validator(ValidateOperationId, "ID"))->type_name("ID");

		app.add_flag("--shader-symbols", arguments.bShaderSymbols, "Export symbols with cook.shaders, cook.all or cook.workspace.");

		app.add_flag("--force-recook", "Request a complete recook.");

		app.add_flag("--confirm-force-recook", arguments.bForceRecookConfirmed, "Confirm replacement of cooked output.");

		app.option_defaults()->group("Cleanup");

		app.add_option_function<std::string>(
		    "--clean-scope",
		    std::bind_front(ParseCleanScope, std::ref(arguments)),
		    "Clean scope: cooked, build-tree, artifacts, workspace-state, deps, logs or clean-all.");

		app.add_flag("--confirm-clean", arguments.bCleanConfirmed, "Confirm cleanup of the selected generated output.");

		app.option_defaults()->group("GPU diagnostics");
		CLI::Option* gpuCrash = app.add_option_function<std::string>("--gpu-crash-debugging", std::bind_front(SelectGpuCrashMode, std::ref(arguments)), "GPU crash collection for scene launch.");
		gpuCrash->check(CLI::IsMember({"off", "aftermath"}));

		CLI::Option* shaderExport = app.add_option("--shader-export", arguments.ShaderExportPath, "Absolute shader-export.json for GPU crash correlation.");
		shaderExport->type_name("PATH");

		for (CLI::Option* option : app.get_options())
		{
			option->trigger_on_parse()->take_last();
		}

		for (CLI::Option* option : {gpuCrash, shaderExport})
		{
			const auto validateOnce = std::bind_front(ValidateSingleUseOption, std::cref(app), option);
			option->check(CLI::Validator(validateOnce, "single occurrence"));
		}
	}

	static std::vector<std::string> PrepareLauncherArguments(const CLI::App& app, int argc, char** argv)
	{
		std::vector<std::string> arguments;
		for (int index = 1; index < argc; ++index)
		{
			const std::string argument = std::string_view(argv[index]) == "/?" ? "-h" : argv[index];
			const CLI::Option* option = app.get_option_no_throw(argument);
			if (option == nullptr)
			{
				// Keep exact option spellings; CLI11 otherwise accepts assignments and short-option clusters.
				arguments.push_back(std::string(UnrecognizedArgumentPrefix) + argument);
				break;
			}
			arguments.push_back(argument);
			const bool bRequiresValue = option->get_expected_min() > 0;
			const bool bHasNextArgument = index + 1 < argc;
			const bool bAcceptsValue = option->get_expected_max() > 0 && bHasNextArgument;
			const bool bOptionalValue = bAcceptsValue && argv[index + 1][0] != '-';
			if (bHasNextArgument && (bRequiresValue || bOptionalValue))
			{
				arguments.emplace_back(argv[++index]);
			}
		}
		std::ranges::reverse(arguments);
		return arguments;
	}

	bool ParseLauncherShellArguments(int argc, char** argv, LauncherShellArguments& outArguments, std::ostream& error)
	{
		CLI::App app("Build, cook and run Sparkle workspaces.", "SparkleLauncher");
		ConfigureLauncherOptions(app, outArguments);
		try
		{
			std::vector<std::string> arguments = PrepareLauncherArguments(app, argc, argv);
			app.parse(arguments);
		}
		catch (const CLI::ParseError& failure)
		{
			error << "SparkleLauncher: " << failure.what() << '\n';
			return false;
		}
		const std::vector<std::string> remaining = app.remaining();
		if (!remaining.empty())
		{
			error << "SparkleLauncher: unexpected argument '" << remaining.front().substr(UnrecognizedArgumentPrefix.size()) << "'.\n";
			return false;
		}
		if (app.count("--force-recook") > 0)
		{
			outArguments.RequestedCookMode = CookMode::Force;
		}

		if (!outArguments.DryRunOperationId.empty() && !outArguments.RunOperationId.empty())
		{
			error << "SparkleLauncher: --dry-run and --run are mutually exclusive.\n";
			return false;
		}
		const std::string& operationId = outArguments.RunOperationId.empty() ? outArguments.DryRunOperationId : outArguments.RunOperationId;
		if (outArguments.bShaderSymbols && operationId != "cook.shaders" && operationId != "cook.all" && operationId != "cook.workspace")
		{
			error << "SparkleLauncher: --shader-symbols requires cook.shaders, cook.all or cook.workspace.\n";
			return false;
		}
		if (!outArguments.LevelId.empty() && !operationId.empty() && operationId != "levels.sync" && operationId != "levels.run")
		{
			error << "SparkleLauncher: --level applies only to levels.sync and levels.run; it does not select cook scopes.\n";
			return false;
		}
		return true;
	}

	void PrintLauncherShellUsage(std::ostream& output)
	{
		LauncherShellArguments arguments;
		CLI::App app("Build, cook and run Sparkle workspaces.", "SparkleLauncher");
		ConfigureLauncherOptions(app, arguments);
		output << app.help();
	}
}
