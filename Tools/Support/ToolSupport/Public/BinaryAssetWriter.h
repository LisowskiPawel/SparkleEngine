#pragma once

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/BinaryStreamWriter.h"
#include "Core/Public/Files/FileUtils.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace SparkleTools
{
	template <typename THeader, typename... TElement> void WriteBinaryAssetFile(const std::filesystem::path& path, const THeader& header, const std::vector<TElement>&... arrays)
	{
		std::string errorMessage;
		std::ofstream output;
		const bool bOutputOpened = Files::TryOpenBinaryOutput(path, output, errorMessage);
		const bool bHeaderWritten = bOutputOpened && Files::BinaryStreamWriter::WriteValue(output, header, errorMessage);
		const bool bArraysWritten = bHeaderWritten && (Files::BinaryStreamWriter::WriteArray(output, arrays, errorMessage) && ...);
		const bool bOutputClosed = bArraysWritten && Files::TryCloseOutput(output, path, errorMessage);
		if (!bOutputClosed)
		{
			throw Diagnostics::Error(errorMessage);
		}
	}
}
