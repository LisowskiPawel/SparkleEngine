#pragma once

#include "TextureCookRequestList.h"

#include <cstddef>
#include <filesystem>
#include <vector>

struct TextureCookBatchItemResult;

namespace Files
{
	class FileSetPublisher;
}

class TextureCookRequestBatchProcessor final
{
public:
	int CookRequestFile(const std::filesystem::path& requestFilePath) const;

private:
	static std::size_t ReportFailures(const std::vector<TextureCookRequest>& requests, const std::vector<TextureCookBatchItemResult>& results);
	static void PublishGeneration(const std::vector<TextureCookRequest>& requests, const std::vector<TextureCookBatchItemResult>& results, Files::FileSetPublisher& publisher);
	static void CleanupStagedOutputs(const std::vector<TextureCookBatchItemResult>& results);
};
