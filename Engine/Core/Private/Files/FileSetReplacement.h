#pragma once

#include "Core/Public/Files/FileSetPublication.h"

#include <vector>

namespace Files
{
	struct FileSetReplacement final
	{
		FilePublication File;
		bool bPreserved = false;
		bool bPublished = false;
	};

	std::vector<FileSetReplacement> PlanFileSetReplacements(std::span<const FilePublication> files, std::span<const std::filesystem::path> roots);
	void ReplaceFileSet(std::span<FileSetReplacement> replacements);
	void DiscardFileSetBackups(std::span<const FileSetReplacement> replacements);
	void RollbackFileSet(std::span<const FileSetReplacement> replacements);
}
