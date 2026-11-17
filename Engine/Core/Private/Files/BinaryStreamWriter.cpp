#include "PCH.h"

#include "Core/Public/Files/BinaryStreamWriter.h"

bool Files::BinaryStreamWriter::WriteBytes(std::ostream& output, const void* bytes, std::size_t byteCount, std::string& outErrorMessage)
{
	if (byteCount > static_cast<std::size_t>((std::numeric_limits<std::streamsize>::max)()))
	{
		outErrorMessage = "Binary payload exceeds the output stream size limit";
		return false;
	}
	if (byteCount == 0)
	{
		return true;
	}

	output.write(reinterpret_cast<const char*>(bytes), static_cast<std::streamsize>(byteCount));
	if (output.good())
	{
		return true;
	}

	outErrorMessage = "Failed to write binary bytes to output stream";
	return false;
}
