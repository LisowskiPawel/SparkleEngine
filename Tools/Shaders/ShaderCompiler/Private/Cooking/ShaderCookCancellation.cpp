#include "PCH.h"

#include "Cooking/ShaderCookCancellation.h"

#include "Core/Public/Diagnostics/Error.h"

#include <format>
#include <system_error>

bool IsShaderCookCancelled(const std::filesystem::path& signalPath)
{
	if (signalPath.empty())
	{
		return false;
	}

	std::error_code errorCode;
	const bool requested = std::filesystem::is_regular_file(signalPath, errorCode);
	if (errorCode && errorCode != std::errc::no_such_file_or_directory)
	{
		throw Diagnostics::Error(std::format("Failed to inspect shader cook cancellation signal '{}': {}", signalPath.string(), errorCode.message()));
	}
	return requested;
}

void ThrowIfShaderCookCancelled(const std::filesystem::path& signalPath)
{
	if (IsShaderCookCancelled(signalPath))
	{
		throw Diagnostics::Error("Shader cook was cancelled before publication.");
	}
}
