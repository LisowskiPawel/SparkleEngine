#include "PCH.h"

#include "Cooking/CookTextureRequestFile.h"

#include "Constants/TextureCookerConstants.h"
#include "Cooking/TextureCookBatchExecutor.h"
#include "TextureCookRequestList.h"
#include "TaskTypes.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "ToolConsole.h"
#include "ToolWorkProgressWriter.h"
#include "SourceAssetPaths.h"

#include <iostream>
#include <exception>
#include <vector>

static std::vector<Files::FilePublication> BuildPublication(const std::vector<TextureCookRequest>& requests)
{
	std::vector<Files::FilePublication> publication;
	publication.reserve(requests.size());
	for (const auto& request : requests)
	{
		publication.push_back({Files::BuildTemporaryPath(request.outputPath, ".cook-generation"), request.outputPath});
	}
	return publication;
}

static std::vector<std::filesystem::path> CollectProtectedInputs(const std::vector<TextureCookRequest>& requests, const std::filesystem::path& requestFilePath)
{
	std::vector<std::filesystem::path> inputs{requestFilePath};
	for (const auto& request : requests)
	{
		const auto source = SparkleTools::ResolveSourceAssetPath(request.sourcePath, SparkleTools::SourceAssetType::Texture);
		if (!source)
		{
			throw Diagnostics::Error("Unable to resolve source texture.");
		}
		inputs.push_back(*source);
	}
	return inputs;
}

static std::size_t ReportFailures(const std::vector<TextureCookRequest>& requests, const std::vector<TaskResult>& results)
{
	std::size_t failureCount = 0;
	for (std::size_t index = 0; index < requests.size(); ++index)
	{
		const TaskResult& result = results[index];
		if (result.GetOutcome() == TaskOutcome::Succeeded)
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
		        ToolConsole::QuotedField("reason", result.GetDiagnostic())});
	}

	return failureCount;
}

int CookTextureRequestFile(const std::filesystem::path& requestFilePath)
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

	try
	{
		const auto publication = BuildPublication(requests);
		const auto inputs = CollectProtectedInputs(requests, requestFilePath);
		std::vector<std::filesystem::path> roots;
		for (const auto& request : requests)
		{
			const auto root = std::filesystem::absolute(request.outputPath).parent_path();
			roots.push_back(root);
		}
		Files::FileSetPublisher publisher(roots);
		std::string fileError;
		if (!publisher.ValidatePaths(publication, inputs, fileError))
		{
			throw Diagnostics::Error(fileError);
		}

		TextureCookBatchExecutor executor(
		    requests,
		    publication,
		    textureCookMemoryBudget,
		    [&progress, &requests, totalWork](std::size_t requestIndex, std::size_t completed)
		    { progress.Report("Cooked texture", Strings::ToNarrow(requests[requestIndex].sourcePath.filename().wstring()), completed, totalWork); });
		const std::vector<TaskResult>& results = executor.Execute();

		if (ReportFailures(requests, results) != 0)
		{
			return TextureCookerConstants::ExitCookFailed;
		}

		progress.Report("Publishing cooked textures", requests.size(), totalWork);

		if (!publisher.Publish(publication, fileError))
		{
			throw Diagnostics::Error(fileError);
		}
	}
	catch (const std::exception& error)
	{
		ToolConsole::Error("Texture cook generation failed: " + std::string(error.what()));
		return TextureCookerConstants::ExitCookFailed;
	}

	progress.Report("Cooked textures published", totalWork, totalWork);

	ToolConsole::Message(std::cout, ToolConsoleSeverity::Info, "Cooked texture generation", {ToolConsole::Field("textures", std::to_string(requests.size()))});
	return TextureCookerConstants::ExitSuccess;
}
