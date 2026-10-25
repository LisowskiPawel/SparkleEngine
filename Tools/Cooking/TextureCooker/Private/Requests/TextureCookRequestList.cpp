#include "TextureCookRequestList.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Strings/StringUtils.h"
#include "TextureCookRequestCodec.h"

#include <algorithm>
#include <fstream>
#include <sstream>

static void SortForSerialization(std::vector<TextureCookRequest>& requests)
{
	std::ranges::sort(
	    requests,
	    [](const TextureCookRequest& lhs, const TextureCookRequest& rhs) { return lhs.assetId != rhs.assetId ? lhs.assetId < rhs.assetId : lhs.outputPath.native() < rhs.outputPath.native(); });
}

void WriteTextureCookRequestList(const std::filesystem::path& outputPath, const std::vector<TextureCookRequest>& requests)
{
	if (outputPath.empty())
	{
		throw Diagnostics::Error("Texture cook request output path is empty.");
	}

	std::vector<TextureCookRequest> sortedRequests = requests;
	SortForSerialization(sortedRequests);
	std::ostringstream output;
	output << TextureCookRequestCodec::GetHeader() << '\n';
	for (const TextureCookRequest& request : sortedRequests)
	{
		ValidateTextureCookRequest(request);
		output << TextureCookRequestCodec::FormatLine(request) << '\n';
	}

	std::string fileError;
	if (!Files::TryWriteAllText(outputPath, output.str(), fileError))
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
	bool foundHeader = false;
	std::size_t lineNumber = 0;
	for (std::string line; std::getline(input, line);)
	{
		++lineNumber;
		const std::string trimmedLine = Strings::TrimCopy(line);
		if (trimmedLine.empty())
		{
			continue;
		}
		if (!foundHeader)
		{
			if (!TextureCookRequestCodec::IsHeader(trimmedLine))
			{
				throw Diagnostics::Error("Texture cook request file '" + Strings::ToNarrow(inputPath.wstring()) + "' has an invalid header.");
			}
			foundHeader = true;
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

	if (!foundHeader)
	{
		throw Diagnostics::Error("Texture cook request file '" + Strings::ToNarrow(inputPath.wstring()) + "' is empty.");
	}
	std::vector<TextureCookRequest> requests = requestSet.ReleaseRequests();
	std::ranges::sort(requests, {}, &TextureCookRequest::assetId);
	return requests;
}
