#include "TextureCookRequestList.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Strings/StringUtils.h"
#include "TextureCookRequestCodec.h"

#include <fstream>
#include <sstream>

void WriteTextureCookRequestList(const std::filesystem::path& outputPath, TextureCookRequestSet&& requests)
{
	if (outputPath.empty())
	{
		throw Diagnostics::Error("Texture cook request output path is empty.");
	}

	const auto sortedRequests = requests.ReleaseRequests();
	std::ostringstream output;
	output << TextureCookRequestCodec::Header << '\n';
	for (const TextureCookRequest& request : sortedRequests)
	{
		output << TextureCookRequestCodec::FormatLine(request) << '\n';
	}

	std::string fileError;
	if (!Files::TryWriteAllTextAtomic(outputPath, output.str(), fileError))
	{
		throw Diagnostics::Error(fileError);
	}
}

std::vector<TextureCookRequest> LoadTextureCookRequestList(const std::filesystem::path& inputPath)
{
	std::ifstream input(inputPath);
	if (!input.is_open())
	{
		throw Diagnostics::Error("Failed to open texture cook request file '" + Strings::ToNarrow(inputPath.wstring()) + "'.");
	}

	TextureCookRequestSet requestSet;
	bool bFoundHeader = false;
	std::size_t lineNumber = 0;
	for (std::string line; std::getline(input, line);)
	{
		++lineNumber;
		const std::string trimmedLine = Strings::TrimCopy(line);
		if (trimmedLine.empty())
		{
			continue;
		}
		if (!bFoundHeader)
		{
			if (trimmedLine != TextureCookRequestCodec::Header)
			{
				throw Diagnostics::Error("Texture cook request file '" + Strings::ToNarrow(inputPath.wstring()) + "' has an invalid header.");
			}
			bFoundHeader = true;
			continue;
		}

		try
		{
			requestSet.Add(TextureCookRequestCodec::ParseLine(trimmedLine));
		}
		catch (const Diagnostics::Error& error)
		{
			throw Diagnostics::Error(std::string(error.what()) + " File: '" + Strings::ToNarrow(inputPath.wstring()) + "', line " + std::to_string(lineNumber) + ".");
		}
	}

	if (!bFoundHeader)
	{
		throw Diagnostics::Error("Texture cook request file '" + Strings::ToNarrow(inputPath.wstring()) + "' is empty.");
	}
	return requestSet.ReleaseRequests();
}
