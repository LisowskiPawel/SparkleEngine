#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>

namespace SparkleTools
{
	enum class SourceAssetType : std::uint8_t
	{
		Shader,
		Texture,
		Mesh
	};

	enum class SourceAssetRoot : std::uint8_t
	{
		Project,
		Engine
	};

	std::filesystem::path GetSourceAssetRoot(SourceAssetType type, SourceAssetRoot root);
	// Absolute files are accepted directly; relative inputs search project assets before engine assets.
	std::optional<std::filesystem::path> ResolveSourceAssetPath(const std::filesystem::path& inputPath, SourceAssetType type);
	std::filesystem::path ImportedTextureCacheRoot();
}
