#include "Cli/ShaderCompilerCli.h"

#include "Cli/CookShadersCommand.h"
#include "Cli/InspectShaderCommand.h"
#include "Cli/ListBackendsCommand.h"
#include "Cli/ListShadersCommand.h"
#include "Cli/ListTargetsCommand.h"
#include "Cli/ShaderCookOptions.h"
#include "Constants/ShaderCompilerConstants.h"
#include "Cooking/ShaderCookSettings.h"
#include "ToolConsole.h"

#include <CLI/CLI.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

static constexpr std::string_view kUnrecognizedArgumentPrefix = "unrecognized:";

static void AppendOptionArguments(const CLI::App& command, int argc, char** argv, std::vector<std::string>& arguments)
{
	for (int index = 2; index < argc; ++index)
	{
		const std::string argument = argv[index];
		const CLI::Option* option = command.get_option_no_throw(argument);
		if (option == nullptr || option == command.get_help_ptr())
		{
			// Preserve exact spellings and stop at the first unknown token; CLI11 otherwise accepts assignments and clusters.
			arguments.push_back(std::string(kUnrecognizedArgumentPrefix) + argument);
			break;
		}
		arguments.push_back(argument);
		if (option->get_expected_min() > 0 && index + 1 < argc)
		{
			arguments.emplace_back(argv[++index]);
		}
	}
}

static std::vector<std::string> NormalizeShaderArguments(const CLI::App& app, int argc, char** argv)
{
	std::vector<std::string> arguments;
	if (argc < 2)
	{
		return arguments;
	}

	arguments.emplace_back(argv[1]);
	const CLI::App* command = app.get_subcommand_no_throw(argv[1]);
	if (command == nullptr)
	{
		return arguments;
	}

	const auto options = command->get_options();
	const bool bHasPositional = std::ranges::any_of(options, [](const CLI::Option* option) { return option->get_positional(); });
	if (bHasPositional)
	{
		// Inspect IDs are literal, including option-like values and command names.
		arguments.emplace_back("--");
		for (int index = 2; index < argc; ++index)
		{
			arguments.emplace_back(argv[index]);
		}
	}
	else
	{
		AppendOptionArguments(*command, argc, argv, arguments);
	}
	std::ranges::reverse(arguments);
	return arguments;
}

static bool TryPrintHelp(const CLI::App& app, const CLI::App& cook, int argc, char** argv)
{
	if (argc < 2)
	{
		return false;
	}

	const bool bTopLevelHelp = app.get_help_ptr()->check_name(argv[1]) || std::string_view(argv[1]) == "/?";
	if (bTopLevelHelp)
	{
		std::cout << app.help("", CLI::AppFormatMode::All);
		return true;
	}
	const bool bCookHelp = argc == 3 && app.get_subcommand_no_throw(argv[1]) == &cook;
	if (bCookHelp && cook.get_help_ptr()->check_name(argv[2]))
	{
		std::cout << cook.help(app.get_name());
		return true;
	}
	return false;
}

static void PrintArgumentFailure(const CLI::App& app, const CLI::App& cook, const CLI::ParseError& failure)
{
	std::string reason = failure.what();
	const std::size_t prefix = reason.find(kUnrecognizedArgumentPrefix);
	if (prefix != std::string::npos)
	{
		reason.erase(prefix, kUnrecognizedArgumentPrefix.size());
	}
	if (cook.parsed())
	{
		ToolConsole::Message(std::cerr, ToolConsoleSeverity::Error, "Invalid shader cook arguments", {ToolConsole::QuotedField("reason", reason)});
		return;
	}

	std::cerr << "ShaderCompiler: " << reason << '\n';
	if (app.get_subcommands().empty())
	{
		std::cerr << app.help();
	}
}

int RunShaderCompiler(int argc, char** argv)
{
	CLI::App app("Compile and inspect typed shader registrations.", "ShaderCompiler");
	app.set_help_flag("-h,--help", "Show help and exit; top level also accepts /?.");
	app.require_subcommand(1, 1);
	ShaderCookSettings settings;
	int exitCode = kExitCodeSuccess;
	CLI::App* cook = app.add_subcommand("cook", "Compile typed shader registrations into GlobalShaderMap.smap and CookedShaderLibrary.slib.");
	ConfigureShaderCookOptions(*cook, settings);

	cook->callback(
	    [&]
	    {
		    ValidateShaderCookSelection(settings);
		    exitCode = CookShaders(settings);
	    });

	app.add_subcommand("list-backends", "List compiler backends and their capabilities.")->callback([&] { exitCode = ListBackends(); });
	app.add_subcommand("list-targets", "List shader codegen target names.")->callback([&] { exitCode = ListShaderTargets(); });

	CLI::App* shaders = app.add_subcommand("list-shaders", "List typed shader registrations or validate their contracts.");
	bool bValidateOnly = false;
	shaders->add_flag("--validate", bValidateOnly, "Validate registered shader contracts without listing them.")->multi_option_policy(CLI::MultiOptionPolicy::Throw);
	shaders->callback([&] { exitCode = ListShaders(bValidateOnly); });

	CLI::App* inspect = app.add_subcommand("inspect-shader", "Inspect cooked map and library records for a registered shader.");
	std::string shaderId;
	inspect->add_option("shader-id", shaderId, "Registered shader ID.")->required();
	inspect->callback([&] { exitCode = InspectShader(shaderId); });

	const auto declaredCommands = app.get_subcommands({});
	for (CLI::App* command : declaredCommands)
	{
		command->subcommand_fallthrough(false);
		if (command != cook)
		{
			command->set_help_flag("");
		}
	}

	const bool bHelpPrinted = TryPrintHelp(app, *cook, argc, argv);
	if (bHelpPrinted)
	{
		return kExitCodeSuccess;
	}

	try
	{
		std::vector<std::string> arguments = NormalizeShaderArguments(app, argc, argv);
		app.parse(arguments);
	}
	catch (const CLI::ParseError& failure)
	{
		PrintArgumentFailure(app, *cook, failure);
		return kExitCodeUsage;
	}
	return exitCode;
}
