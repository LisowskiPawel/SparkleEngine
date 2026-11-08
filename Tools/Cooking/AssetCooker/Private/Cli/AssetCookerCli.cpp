#include "AssetCookerCli.h"

#include "../Api/AssetCookerService.h"

#include "ToolConsole.h"

#include <CLI/CLI.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

struct AssetCookerRequest final
{
	std::string projectName;
	std::string configuration = "DevelopmentGame";
	std::string toolProfile;
	std::string workspaceRoot;
	AssetCookerCategory category = AssetCookerCategory::All;
};

static std::string UnescapeArgument(std::string value)
{
	return value.starts_with(' ') ? value.substr(1) : value;
}

static void ConfigureCookOptions(CLI::App& command, AssetCookerRequest& request, bool bRequiresProject)
{
	command.validate_positionals()->allow_extras(CLI::ExtrasMode::ErrorImmediately);

	command.add_option("configuration", request.configuration, "Build configuration, in any position; repeated configurations use the last value.")
	    ->check(CLI::IsMember({"DebugEditor", "DebugGame", "DevelopmentEditor", "DevelopmentGame", "ShippingEditor", "ShippingGame"}))
	    ->expected(0, 1)
	    ->allow_extra_args()
	    ->capture_default_str();

	command.add_option("ProjectName", request.projectName, "Project name (also ALL, All or all).")
	    ->expected(0, 1)
	    ->allow_extra_args()
	    ->check(CLI::Validator([&request](std::string&) { return request.projectName.empty() ? "" : "project already selected"; }, ""))
	    ->required(bRequiresProject);

	command.add_option("--tool-profile", request.toolProfile, "Editor tool profile; defaults to the editor profile matching the configuration.")->type_name("EDITOR_PROFILE");
	command.add_option("--root", request.workspaceRoot, "Workspace root; defaults to discovery from the current directory.")->type_name("PATH");

	for (CLI::Option* option : command.get_options())
	{
		option->trigger_on_parse()->take_last()->transform(UnescapeArgument);
	}
}

static void ConfigureCookCommands(CLI::App& app, AssetCookerRequest& request)
{
	app.set_help_flag();
	app.allow_windows_style_options(false);

	CLI::App* project = app.add_subcommand("cook-project", "Cook all categories; omit the project or use ALL to cook every project.");
	app.add_subcommand("cook-shaders", "Cook project shaders.")->callback([&request] { request.category = AssetCookerCategory::Shaders; });
	app.add_subcommand("cook-textures", "Cook project textures.")->callback([&request] { request.category = AssetCookerCategory::Textures; });
	app.add_subcommand("cook-assets", "Cook project scenes, meshes and materials.")->callback([&request] { request.category = AssetCookerCategory::SceneAssets; });

	for (CLI::App* command : app.get_subcommands({}))
	{
		ConfigureCookOptions(*command, request, command != project);
	}
}

static std::vector<std::string> EscapeLiteralArguments(const CLI::App& command, int argc, char** argv)
{
	// Escape literal tokens independently. CLI11 alone consumes option values and selects positionals.
	std::vector<std::string> arguments(argv + 2, argv + argc);

	std::ranges::transform(
	    arguments,
	    arguments.begin(),
	    [&command](const std::string& value)
	    {
		    const CLI::Option* option = command.get_option_no_throw(value);
		    return option != nullptr && !option->get_positional() ? value : " " + value;
	    });

	std::ranges::reverse(arguments);
	return arguments;
}

static bool ParseCookRequest(CLI::App& app, int argc, char** argv, AssetCookerRequest& request)
{
	CLI::App* command = argc > 1 ? app.get_subcommand_no_throw(argv[1]) : nullptr;
	if (command == nullptr)
	{
		return false;
	}

	std::vector<std::string> arguments = EscapeLiteralArguments(*command, argc, argv);
	try
	{
		command->parse(arguments);
		const CLI::Option* project = command->get_option("ProjectName");
		return !project->get_required() || !request.projectName.empty();
	}
	catch (const CLI::ArgumentMismatch& failure)
	{
		const std::string_view message = failure.what();
		const CLI::Option* root = command->get_option("--root");
		const bool bMissingRoot = message.starts_with(root->get_name());
		std::cerr << "AssetCooker: " << (bMissingRoot ? "--root requires a path." : "--tool-profile requires an editor profile.") << '\n';
	}
	catch (const CLI::ExtrasError&)
	{
		std::cerr << "AssetCooker: unexpected argument '" << UnescapeArgument(arguments.back()) << "'.\n";
	}
	catch (const CLI::RequiredError&)
	{
	}

	return false;
}

static AssetCookerServiceResult CookProjectContent(const AssetCookerRequest& request)
{
	const char* projectName = request.projectName.empty() ? nullptr : request.projectName.c_str();

	AssetCookerService service(
	    request.workspaceRoot.empty() ? nullptr : request.workspaceRoot.c_str(),
	    projectName,
	    request.configuration.c_str(),
	    request.toolProfile.empty() ? nullptr : request.toolProfile.c_str());

	return service.Cook(projectName, request.configuration.c_str(), request.category);
}

static void PrintCookResult(const AssetCookerServiceResult& result)
{
	for (const AssetCookerDiagnosticRecord& diagnostic : result.diagnostics)
	{
		if (diagnostic.sourcePath.empty())
		{
			ToolConsole::Error(diagnostic.message);
			continue;
		}

		ToolConsole::Message(std::cerr, ToolConsoleSeverity::Error, diagnostic.message, {ToolConsole::QuotedField("source", diagnostic.sourcePath)});
	}

	if (result.exitCode == 0)
	{
		ToolConsole::Message(std::cout, ToolConsoleSeverity::Info, "Cook completed", {ToolConsole::Field("products", std::to_string(result.outputs.size()))});
	}
}

int RunAssetCooker(int argc, char** argv)
{
	AssetCookerRequest request;
	CLI::App app("Cook project content. Top-level help: --help, -h, /? or /h.", "AssetCooker");
	ConfigureCookCommands(app, request);

	const bool bParsed = ParseCookRequest(app, argc, argv, request);
	if (!bParsed)
	{
		std::cerr << app.help("", CLI::AppFormatMode::All);
		return 1;
	}

	const AssetCookerServiceResult result = CookProjectContent(request);

	PrintCookResult(result);
	return result.exitCode;
}
