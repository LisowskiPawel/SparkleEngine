#include "Startup/LauncherArguments.h"

#include "SparkleLauncher/BuildProfileCatalog.h"
#include "Core/Public/Strings/StringUtils.h"

#include <CLI/CLI.hpp>

#include <algorithm>
#include <functional>
#include <ostream>
#include <string_view>
#include <vector>

namespace SparkleLauncher
{
	static constexpr std::string_view UnrecognizedArgumentPrefix = "---";

	struct LauncherOptionGroups final
	{
		CLI::App& Gui;
		CLI::App& WorkspaceRoot;
	};

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

	static std::string RestoreArgumentValue(std::string& value)
	{
		if (value.starts_with(' '))
		{
			value.erase(0, 1);
		}
		return {};
	}

	static LauncherOptionGroups ConfigureLauncherOptions(CLI::App& app, LauncherShellArguments& arguments)
	{
		app.set_help_flag();
		app.allow_windows_style_options(false);
		app.allow_extras()->prefix_command();
		// These options leave GUI startup selected; the root subgroup also supports separate GUI parsing.
		CLI::App* guiOptions = app.add_option_group("Workspace and confirmation");
		CLI::App* rootOptions = guiOptions->add_option_group("+Workspace root");
		rootOptions->add_option("--root", arguments.StartPath, "Workspace root.")->type_name("PATH")->group("Workspace");
		app.add_flag("--help,-h", arguments.bShowHelp, "Show help (also /?).");

		guiOptions->option_defaults()->group("Workspace");

		guiOptions->add_option("--editor-profile", arguments.EditorProfile, "Editor build profile.")->check(CLI::IsMember(GetBuildProfileNames(BuildProfileTarget::Editor)))->capture_default_str();

		guiOptions->add_option("--game-profile", arguments.GameProfile, "Game build profile.")->check(CLI::IsMember(GetBuildProfileNames(BuildProfileTarget::Game)))->capture_default_str();

		guiOptions->add_option_function<std::string>("--ide", std::bind_front(ParseWorkspaceIde, std::ref(arguments)), "Workspace IDE: visual-studio or rider.");

		guiOptions->add_option_function<std::string>("--compiler", std::bind_front(ParseWorkspaceCompiler, std::ref(arguments)), "Workspace compiler: msvc or clang-cl.");

		app.option_defaults()->group("Scene selection");
		app.add_option("--level", arguments.LevelId, "Catalog level for levels.sync or levels.run.")->type_name("ID");

		app.add_option_function<std::string>("--product", std::bind_front(SelectLaunchProduct, std::ref(arguments)), "Application to launch.")->check(CLI::IsMember({"editor", "game"}));

		app.add_option("--graphics-api", arguments.GraphicsApi, "Graphics API for scene launch: d3d12 or vulkan.")->capture_default_str();

		app.option_defaults()->group("Operations");
		guiOptions->option_defaults()->group("Operations");

		app.add_option("--dry-run", arguments.DryRunOperationId, "Preview an operation; defaults to workspace.sync-code when its ID is omitted.")
		    ->expected(0, 1)
		    ->default_str("workspace.sync-code")
		    ->transform(CLI::Validator(RestoreArgumentValue, ""))
		    ->type_name("ID");

		app.add_option("--run", arguments.RunOperationId, "Execute an operation.")->check(CLI::Validator(ValidateOperationId, ""))->type_name("ID");

		guiOptions->add_flag("--shader-symbols", arguments.bShaderSymbols, "Export symbols with cook.shaders, cook.all or cook.workspace.");

		guiOptions->add_flag("--force-recook", "Request a complete recook.");

		guiOptions->add_flag("--confirm-force-recook", arguments.bForceRecookConfirmed, "Confirm replacement of cooked output.");

		guiOptions->option_defaults()->group("Cleanup");

		guiOptions->add_option_function<std::string>(
		    "--clean-scope",
		    std::bind_front(ParseCleanScope, std::ref(arguments)),
		    "Clean scope: cooked, build-tree, artifacts, workspace-state, deps, logs or clean-all.");

		guiOptions->add_flag("--confirm-clean", arguments.bCleanConfirmed, "Confirm cleanup of the selected generated output.");

		app.option_defaults()->group("GPU diagnostics");

		CLI::Option* gpuCrash = app.add_option_function<std::string>("--gpu-crash-debugging", std::bind_front(SelectGpuCrashMode, std::ref(arguments)), "GPU crash collection for scene launch.");

		const auto validateGpuCrashUse = std::bind_front(ValidateSingleUseOption, std::cref(app), gpuCrash);
		gpuCrash->check(CLI::Validator(validateGpuCrashUse, ""))->check(CLI::IsMember({"off", "aftermath"}));

		CLI::Option* shaderExport = app.add_option("--shader-export", arguments.ShaderExportPath, "Absolute shader-export.json for GPU crash correlation.");
		const auto validateShaderExportUse = std::bind_front(ValidateSingleUseOption, std::cref(app), shaderExport);
		shaderExport->check(CLI::Validator(validateShaderExportUse, ""))->type_name("PATH");

		for (CLI::Option* option : app.get_options())
		{
			option->trigger_on_parse()->take_last();
		}
		return {.Gui = *guiOptions, .WorkspaceRoot = *rootOptions};
	}

	bool ShouldRunLauncherShell(int argc, char** argv)
	{
		LauncherShellArguments arguments;
		CLI::App app;
		const LauncherOptionGroups groups = ConfigureLauncherOptions(app, arguments);
		for (int index = 1; index < argc; ++index)
		{
			const std::string argument = std::string_view(argv[index]) == "/?" ? "-h" : argv[index];
			const bool bKnownOption = app.get_option_no_throw(argument) != nullptr;
			const bool bGuiOption = groups.Gui.get_option_no_throw(argument) != nullptr;
			if (bKnownOption && !bGuiOption)
			{
				return true;
			}
		}
		return false;
	}

	std::filesystem::path ParseLauncherGuiRoot(int argc, char** argv)
	{
		LauncherShellArguments request;
		CLI::App app;
		const LauncherOptionGroups groups = ConfigureLauncherOptions(app, request);
		CLI::Option* root = groups.WorkspaceRoot.get_option("--root");
		root->trigger_on_parse(false)->take_first()->transform(CLI::Validator(RestoreArgumentValue, ""));
		groups.WorkspaceRoot.allow_extras()->prefix_command(false);

		std::vector<std::string> arguments;
		for (int index = 1; index < argc; ++index)
		{
			const std::string argument(argv[index]);
			// GUI startup ignores other arguments. Protect exact spelling and option-like root values.
			arguments.push_back(root->check_name(argument) ? argument : " " + argument);
		}
		std::ranges::reverse(arguments);
		try
		{
			groups.WorkspaceRoot.parse(arguments);
		}
		catch (const CLI::ArgumentMismatch&)
		{
			// A trailing --root has no value; an earlier complete occurrence still wins.
		}
		if (root->results().empty())
		{
			return {};
		}
		const std::string value = root->as<std::string>();
		return Strings::ToWide(value);
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
				// Protect explicit empty optional values from matching an unnamed option group.
				const std::string value(argv[++index]);
				arguments.push_back(bRequiresValue ? value : " " + value);
			}
			else if (option->get_expected_max() > 0 && !bRequiresValue)
			{
				// CLI11 may consume an unknown option as an optional value. Resolve omission from its declaration.
				arguments.push_back(option->get_default_str());
			}
		}
		std::ranges::reverse(arguments);
		return arguments;
	}

	static bool ValidateLauncherOperationArguments(const LauncherShellArguments& arguments, std::ostream& error)
	{
		const bool bHasDryRun = !arguments.DryRunOperationId.empty();
		const bool bHasRun = !arguments.RunOperationId.empty();
		if (bHasDryRun && bHasRun)
		{
			error << "SparkleLauncher: --dry-run and --run are mutually exclusive.\n";
			return false;
		}
		const std::string& operationId = bHasRun ? arguments.RunOperationId : arguments.DryRunOperationId;
		if (arguments.bShaderSymbols)
		{
			const bool bShaderCook = operationId == "cook.shaders" || operationId == "cook.all";
			const bool bSupportsShaderSymbols = bShaderCook || operationId == "cook.workspace";
			if (!bSupportsShaderSymbols)
			{
				error << "SparkleLauncher: --shader-symbols requires cook.shaders, cook.all or cook.workspace.\n";
				return false;
			}
		}
		const bool bHasLevelSelection = !arguments.LevelId.empty() && !operationId.empty();
		const bool bLevelOperation = operationId == "levels.sync" || operationId == "levels.run";
		if (bHasLevelSelection && !bLevelOperation)
		{
			error << "SparkleLauncher: --level applies only to levels.sync and levels.run; it does not select cook scopes.\n";
			return false;
		}
		return true;
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

		return ValidateLauncherOperationArguments(outArguments, error);
	}

	void PrintLauncherShellUsage(std::ostream& output)
	{
		LauncherShellArguments arguments;
		CLI::App app("Build, cook and run Sparkle workspaces.", "SparkleLauncher");
		ConfigureLauncherOptions(app, arguments);
		output << app.help();
	}
}
