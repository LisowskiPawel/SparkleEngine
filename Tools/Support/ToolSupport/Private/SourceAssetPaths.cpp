#include "SourceAssetPaths.h"

#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Paths/ProductUserStatePaths.h"

#include <initializer_list>
#include <string_view>
#include <system_error>

namespace SparkleTools
{
	static std::string_view GetSourceAssetSubdirectory(SourceAssetType type)
	{
		switch (type)
		{
			case SourceAssetType::Shader:
				return "Shaders";
			case SourceAssetType::Texture:
				return "Textures";
			case SourceAssetType::Mesh:
				return "Meshes";
		}
		return {};
	}

	static const std::filesystem::path& GetRoot(SourceAssetRoot root)
	{
		return root == SourceAssetRoot::Project ? Filesystem::GetProjectPath() : Filesystem::GetEnginePath();
	}

	std::filesystem::path GetSourceAssetRoot(SourceAssetType type, SourceAssetRoot root)
	{
		const std::filesystem::path& path = GetRoot(root);
		return path.empty() ? std::filesystem::path{} : path / "Assets" / GetSourceAssetSubdirectory(type);
	}

	static std::optional<std::filesystem::path> ResolveSourceFile(const std::filesystem::path& path)
	{
		std::error_code error;
		return std::filesystem::is_regular_file(path, error) ? std::make_optional(Paths::Normalize(path)) : std::nullopt;
	}

	std::optional<std::filesystem::path> ResolveSourceAssetPath(const std::filesystem::path& inputPath, SourceAssetType type)
	{
		if (inputPath.empty())
		{
			return std::nullopt;
		}
		if (inputPath.is_absolute())
		{
			return ResolveSourceFile(inputPath);
		}
		for (const SourceAssetRoot root : {SourceAssetRoot::Project, SourceAssetRoot::Engine})
		{
			if (GetRoot(root).empty())
			{
				continue;
			}
			if (const auto path = ResolveSourceFile(GetSourceAssetRoot(type, root) / inputPath))
			{
				return path;
			}
			if (const auto path = ResolveSourceFile(GetRoot(root) / "Assets" / inputPath))
			{
				return path;
			}
		}
		return std::nullopt;
	}

	std::filesystem::path ImportedTextureCacheRoot()
	{
		return Filesystem::GetProductUserStatePaths().CacheRoot / "ImportedTextures";
	}
}
