#include "PCH.h"

#include "Cooking/TextureCookRequestBatchProcessor.h"

#include "Constants/TextureCookerConstants.h"
#include "Cooking/TextureCookBatchExecutor.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "ToolConsole.h"
#include "ToolWorkProgress.h"

#include <iostream>

int TextureCookRequestBatchProcessor::CookRequestFile(const std::filesystem::path& requestFilePath) const
{
	std::vector<TextureCookRequest> requests;
	try
	{
		requests = LoadTextureCookRequestList(requestFilePath);
	}
	catch (const Diagnostics::Error& error)
	{
		ToolConsole::Message(
		    std::cerr,
		    ToolConsoleSeverity::Error,
		    "Failed to load texture request file",
		    {ToolConsole::PathField("requestFile", requestFilePath), ToolConsole::QuotedField("reason", error.what())});

		return TextureCookerConstants::ExitLoadRequestFileFailed;
	}

	constexpr std::size_t textureCookMemoryBudget = 1024ull * 1024ull * 1024ull;
	const std::size_t totalWork = requests.size() + 1;
	ToolWorkProgressWriter progress(std::cout);
	progress.Report("Cooking textures", 0, totalWork);

	std::vector<TextureCookBatchItemResult> results;
	try
	{
		std::vector<std::filesystem::path> roots;
		for (const auto& request : requests)
		{
			const auto root = std::filesystem::absolute(request.outputPath).parent_path();
			roots.push_back(root);
		}
		Files::FileSetPublisher publisher(roots);

		results = TextureCookBatchExecutor::Execute(
		    requests,
		    textureCookMemoryBudget,
		    [&progress, &requests, totalWork](std::size_t requestIndex, std::size_t completed)
		    { progress.Report("Cooked texture", requests[requestIndex].sourcePath.filename().string(), completed, totalWork); });

		if (ReportFailures(requests, results) != 0)
		{
			CleanupStagedOutputs(results);
			return TextureCookerConstants::ExitCookFailed;
		}
		progress.Report("Publishing cooked textures", requests.size(), totalWork);

		PublishGeneration(requests, results, publisher);
	}
	catch (const Diagnostics::Error& error)
	{
		CleanupStagedOutputs(results);
		ToolConsole::Error("Failed to publish texture cook generation: " + std::string(error.what()));
		return TextureCookerConstants::ExitCookFailed;
	}
	progress.Report("Cooked textures published", totalWork, totalWork);

	ToolConsole::Message(std::cout, ToolConsoleSeverity::Info, "Cooked texture generation", {ToolConsole::Field("textures", std::to_string(requests.size()))});
	return TextureCookerConstants::ExitSuccess;
}

std::size_t TextureCookRequestBatchProcessor::ReportFailures(const std::vector<TextureCookRequest>& requests, const std::vector<TextureCookBatchItemResult>& results)
{
	std::size_t failureCount = 0;
	for (std::size_t index = 0; index < requests.size(); ++index)
	{
		const TextureCookBatchItemResult& result = results[index];
		if (result.CookResult.GetOutcome() == TaskOutcome::Succeeded)
		{
			continue;
		}

		++failureCount;

		ToolConsole::Message(
		    std::cerr,
		    ToolConsoleSeverity::Error,
		    "Texture cook failed",
		    {ToolConsole::Field("assetId", Formatting::FormatHexUInt64(requests[index].assetId)),
		        ToolConsole::PathField("source", requests[index].sourcePath),
		        ToolConsole::QuotedField("reason", result.CookResult.GetMessage())});
	}

	return failureCount;
}

void TextureCookRequestBatchProcessor::PublishGeneration(const std::vector<TextureCookRequest>& requests, const std::vector<TextureCookBatchItemResult>& results, Files::FileSetPublisher& publisher)
{
	std::vector<Files::FilePublication> publication;
	publication.reserve(requests.size());
	for (std::size_t index = 0; index < requests.size(); ++index)
	{
		publication.push_back({results[index].StagedOutputPath, requests[index].outputPath});
	}

	std::string fileError;
	if (!publisher.Publish(publication, fileError))
	{
		throw Diagnostics::Error(fileError);
	}
}

void TextureCookRequestBatchProcessor::CleanupStagedOutputs(const std::vector<TextureCookBatchItemResult>& results)
{
	for (const TextureCookBatchItemResult& result : results)
	{
		if (!result.StagedOutputPath.empty())
		{
			Files::CleanupTemporaryFile(result.StagedOutputPath);
		}
	}
}
