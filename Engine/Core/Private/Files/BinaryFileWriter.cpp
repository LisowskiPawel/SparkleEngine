#include "PCH.h"

#include "Core/Public/Files/BinaryFileWriter.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"

#include <limits>
#include <string>

Files::BinaryFileWriter::BinaryFileWriter(const std::filesystem::path& path) :
    m_path(path)
{
	std::string error;
	if (!TryOpenBinaryOutput(m_path, m_output, error))
	{
		throw Diagnostics::Error(error);
	}
}

Files::BinaryFileWriter::~BinaryFileWriter() = default;

void Files::BinaryFileWriter::WriteBytes(const void* bytes, std::size_t byteCount, std::string_view failureMessage)
{
	if (byteCount > static_cast<std::size_t>((std::numeric_limits<std::streamsize>::max)()))
	{
		throw Diagnostics::Error(std::string(failureMessage.empty() ? "Binary payload exceeds the output stream size limit" : failureMessage));
	}
	if (byteCount == 0)
	{
		return;
	}

	m_output.write(reinterpret_cast<const char*>(bytes), static_cast<std::streamsize>(byteCount));
	if (!m_output.good())
	{
		throw Diagnostics::Error(std::string(failureMessage.empty() ? "Failed to write binary bytes to output stream" : failureMessage));
	}
}

void Files::BinaryFileWriter::Close()
{
	std::string error;
	if (!TryCloseOutput(m_output, m_path, error))
	{
		throw Diagnostics::Error(error);
	}
}
