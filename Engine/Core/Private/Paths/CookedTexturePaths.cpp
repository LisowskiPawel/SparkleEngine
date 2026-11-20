#include "PCH.h"
#include "Core/Public/Paths/CookedAssetPaths.h"

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
		const std::string extension = inputPath.extension().string();
		if (!Strings::EqualsIgnoreCase(extension, ".stex"))
		{
			return std::nullopt;
		}
		if (inputPath.is_absolute())
		{
			return ResolveTextureFile(inputPath);
		}
		const auto texturePath = GetCookedTextureRootPath() / inputPath;
		if (const auto path = ResolveTextureFile(texturePath))
		{
			return path;
		}
		const auto assetPath = GetCookedAssetRootPath() / inputPath;
		return ResolveTextureFile(assetPath);
	}

	void AppendCookedTexturePaths(std::span<const std::filesystem::path> inputPaths, std::vector<std::filesystem::path>& destination)
	{
		for (const std::filesystem::path& inputPath : inputPaths)
		{
			auto cookedPath = ResolveCookedTexturePath(inputPath);
			auto fallbackPath = Paths::Normalize(inputPath);
			std::filesystem::path normalizedPath = std::move(cookedPath).value_or(std::move(fallbackPath));
			if (!normalizedPath.empty())
			{
				destination.push_back(std::move(normalizedPath));
			}
		}
	}
}
