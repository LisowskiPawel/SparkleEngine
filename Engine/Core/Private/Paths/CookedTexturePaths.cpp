#include "PCH.h"

#include "Core/Public/Paths/FileSystemPaths.h"

#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Strings/StringUtils.h"

#include <system_error>
#include <utility>

namespace Filesystem
{
	static std::optional<std::filesystem::path> ResolveTextureFile(const std::filesystem::path& path)
	{
		std::error_code error;
		if (!std::filesystem::is_regular_file(path, error))
		{
			return std::nullopt;
		}
		return Paths::Normalize(path);
	}

	std::optional<std::filesystem::path> ResolveCookedTexturePath(const std::filesystem::path& inputPath)
	{
		if (!Strings::EqualsIgnoreCase(inputPath.extension().string(), ".stex"))
		{
			return std::nullopt;
		}
		if (inputPath.is_absolute())
		{
			return ResolveTextureFile(inputPath);
		}
		if (const auto path = ResolveTextureFile(GetCookedTextureRootPath() / inputPath))
		{
			return path;
		}
		return ResolveTextureFile(GetCookedAssetRootPath() / inputPath);
	}

	void AppendCookedTexturePaths(std::span<const std::filesystem::path> inputPaths, std::vector<std::filesystem::path>& destination)
	{
		for (const std::filesystem::path& inputPath : inputPaths)
		{
			std::filesystem::path normalizedPath = ResolveCookedTexturePath(inputPath).value_or(Paths::Normalize(inputPath));
			if (!normalizedPath.empty())
			{
				destination.push_back(std::move(normalizedPath));
			}
		}
	}
}
