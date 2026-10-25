#include "PCH.h"

#include "App/TextureCookerApplication.h"

#include "Cli/CookTextureCookRequestFileCommand.h"
#include "Cli/InspectTextureCookRequestFileCommand.h"
#include "Constants/TextureCookerConstants.h"
#include "Core/Public/Strings/StringUtils.h"

#include <filesystem>
#include <iostream>
#include <string_view>

static void PrintUsage(std::ostream& output)
{
	output << "Usage:\n"
	       << "  TextureCooker inspect-request-file <request-file-path>\n"
	       << "  TextureCooker cook-request-file <request-file-path>\n";
}

int RunTextureCooker(int argc, char** argv)
{
	if (argc == 3)
	{
		const std::string_view command(argv[1] != nullptr ? argv[1] : "");
		if (command == TextureCookerConstants::InspectRequestFileCommand)
		{
			return InspectTextureCookRequestFile(Strings::ToWide(std::string_view(argv[2])));
		}
		if (command == TextureCookerConstants::CookRequestFileCommand)
		{
			return CookTextureCookRequestFile(Strings::ToWide(std::string_view(argv[2])));
		}
	}
	PrintUsage(std::cerr);
	return TextureCookerConstants::ExitUsageError;
}
