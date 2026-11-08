#include "PCH.h"

#include "TextureCookRequestBuilder.h"

#include "SourceAssetPaths.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Paths/PathUtils.h"

#include <optional>
#include <string>

static std::string BuildTextureSourceKeyForRoot(std::string_view rootName, const TextureCookRequest& request, const std::filesystem::path& relativePath);
static std::optional<std::filesystem::path> BuildTextureOutputPathForRoot(
    const std::filesystem::path& sourceTexturePath,
    const std::filesystem::path& sourceRoot,
    std::string_view cookedDirectory,
    std::string_view variantSuffix);
static TextureColorSpace ResolveColorSpace(TextureGroup textureGroup) noexcept;
static TextureMipPolicy ResolveMipPolicy(const std::filesystem::path& sourceTexturePath, TextureGroup textureGroup) noexcept;
static TextureMipFilter ResolveMipFilter(TextureGroup textureGroup) noexcept;
static TextureColorProcessingPolicy ResolveColorProcessingPolicy(TextureGroup textureGroup) noexcept;
static TextureDimension ResolveTextureDimension(TextureGroup textureGroup) noexcept;
static std::filesystem::path NormalizeSourceTexturePath(const std::filesystem::path& sourceTexturePath);
static std::string BuildTextureSourceKey(const TextureCookRequest& request);
static std::filesystem::path BuildTextureOutputPath(const TextureCookRequest& request);
static std::string BuildTextureVariantSuffix(const TextureCookRequest& request);

static std::string BuildTextureSourceKeyForRoot(std::string_view rootName, const TextureCookRequest& request, const std::filesystem::path& relativePath)
{
	return std::string(rootName) + ":" + GetTextureColorSpaceName(request.policy.colorSpace) + ":" + GetTextureMipPolicyName(request.policy.mipPolicy) + ":"
	    + GetTextureMipFilterName(request.policy.mipFilter) + ":" + GetTextureColorProcessingPolicyName(request.policy.colorProcessingPolicy) + ":" + GetTextureGroupName(request.policy.textureGroup)
	    + ":" + GetTextureDimensionName(request.policy.dimension) + ":" + GetTextureChannelMaskName(request.policy.channelMask) + ":" + relativePath.generic_string();
}

static std::optional<std::filesystem::path> BuildTextureOutputPathForRoot(
    const std::filesystem::path& sourceTexturePath,
    const std::filesystem::path& sourceRoot,
    std::string_view cookedDirectory,
    std::string_view variantSuffix)
{
	const auto relativePath = Paths::TryMakeRelativeUnderRoot(sourceTexturePath, sourceRoot);
	if (!relativePath)
	{
		return std::nullopt;
	}

	std::filesystem::path cookedRelativePath = std::filesystem::path(cookedDirectory) / *relativePath;
	cookedRelativePath.replace_extension(std::string(variantSuffix) + ".stex");
	return Filesystem::GetCookedTextureRootPath() / cookedRelativePath;
}

TextureCookRequest BuildTextureCookRequest(const std::filesystem::path& sourceTexturePath, TextureGroup textureGroup, TextureChannelMask channelMask)
{
	TextureCookRequest request;
	request.sourcePath = NormalizeSourceTexturePath(sourceTexturePath);
	request.policy.colorSpace = ResolveColorSpace(textureGroup);
	request.policy.mipPolicy = ResolveMipPolicy(request.sourcePath, textureGroup);
	request.policy.mipFilter = ResolveMipFilter(textureGroup);
	request.policy.colorProcessingPolicy = ResolveColorProcessingPolicy(textureGroup);
	request.policy.textureGroup = textureGroup;
	request.policy.dimension = ResolveTextureDimension(textureGroup);
	request.policy.channelMask = channelMask;
	request.assetId = Hash::Fnv1a64(BuildTextureSourceKey(request));
	request.outputPath = BuildTextureOutputPath(request);
	return request;
}

static TextureColorSpace ResolveColorSpace(TextureGroup textureGroup) noexcept
{
	switch (textureGroup)
	{
		case TextureGroup::Diffuse:
		case TextureGroup::Emissive:
		case TextureGroup::SubsurfaceColor:
			return TextureColorSpace::Srgb;

		case TextureGroup::Default:
		case TextureGroup::NormalMap:
		case TextureGroup::Roughness:
		case TextureGroup::Metallic:
		case TextureGroup::AmbientOcclusion:
		case TextureGroup::SubsurfaceStrength:
		case TextureGroup::HdrColor:
		default:
			return TextureColorSpace::Linear;
	}
}

static TextureMipPolicy ResolveMipPolicy(const std::filesystem::path& sourceTexturePath, TextureGroup textureGroup) noexcept
{
	(void) textureGroup;
	if (Paths::GetLowercaseExtension(sourceTexturePath) == L".dds")
	{
		return TextureMipPolicy::PreserveExisting;
	}
	return TextureMipPolicy::Generate;
}

static TextureMipFilter ResolveMipFilter(TextureGroup textureGroup) noexcept
{
	switch (textureGroup)
	{
		case TextureGroup::Diffuse:
			return TextureMipFilter::Kaiser;

		case TextureGroup::NormalMap:
			return TextureMipFilter::NormalAware;

		case TextureGroup::Default:
		case TextureGroup::Roughness:
		case TextureGroup::Metallic:
		case TextureGroup::AmbientOcclusion:
		case TextureGroup::Emissive:
		case TextureGroup::SubsurfaceColor:
		case TextureGroup::SubsurfaceStrength:
		case TextureGroup::HdrColor:
		default:
			return TextureMipFilter::Regular;
	}
}

static TextureColorProcessingPolicy ResolveColorProcessingPolicy(TextureGroup textureGroup) noexcept
{
	return ResolveColorSpace(textureGroup) == TextureColorSpace::Srgb ? TextureColorProcessingPolicy::SrgbLinearize : TextureColorProcessingPolicy::Linear;
}

static TextureDimension ResolveTextureDimension(TextureGroup textureGroup) noexcept
{
	(void) textureGroup;
	return TextureDimension::Texture2D;
}

static std::filesystem::path NormalizeSourceTexturePath(const std::filesystem::path& sourceTexturePath)
{
	{
		const auto resolvedPath = SparkleTools::ResolveSourceAssetPath(sourceTexturePath, SparkleTools::SourceAssetType::Texture);

		if (resolvedPath)
		{
			return *resolvedPath;
		}
	}

	throw Diagnostics::Error("Unable to resolve source texture path '" + sourceTexturePath.string() + "'.");
}

static std::string BuildTextureSourceKey(const TextureCookRequest& request)
{
	const std::filesystem::path& projectRoot = Filesystem::GetProjectPath();
	if (const auto relativePath = Paths::TryMakeRelativeUnderRoot(request.sourcePath, projectRoot))
	{
		return BuildTextureSourceKeyForRoot("project", request, *relativePath);
	}

	const std::filesystem::path& engineRoot = Filesystem::GetEnginePath();
	if (const auto relativePath = Paths::TryMakeRelativeUnderRoot(request.sourcePath, engineRoot))
	{
		return BuildTextureSourceKeyForRoot("engine", request, *relativePath);
	}

	const std::filesystem::path importedTextureRoot = SparkleTools::ImportedTextureCacheRoot();
	if (const auto relativePath = Paths::TryMakeRelativeUnderRoot(request.sourcePath, importedTextureRoot))
	{
		return BuildTextureSourceKeyForRoot("imported", request, *relativePath);
	}

	throw Diagnostics::Error(
	    "Source texture path must be under the project, engine, or imported-texture cache root "
	    "to derive a stable cooked texture id: '"
	    + request.sourcePath.string() + "'.");
}

static std::filesystem::path BuildTextureOutputPath(const TextureCookRequest& request)
{
	const std::string variantSuffix = BuildTextureVariantSuffix(request);
	const std::filesystem::path& projectRoot = Filesystem::GetProjectPath();
	if (const auto outputPath = BuildTextureOutputPathForRoot(request.sourcePath, projectRoot, "Project", variantSuffix))
	{
		return *outputPath;
	}

	const std::filesystem::path& engineRoot = Filesystem::GetEnginePath();
	if (const auto outputPath = BuildTextureOutputPathForRoot(request.sourcePath, engineRoot, "Engine", variantSuffix))
	{
		return *outputPath;
	}

	{
		const auto outputPath = BuildTextureOutputPathForRoot(request.sourcePath, SparkleTools::ImportedTextureCacheRoot(), "Imported", variantSuffix);

		if (outputPath)
		{
			return *outputPath;
		}
	}

	throw Diagnostics::Error(
	    "Source texture path must be under the project, engine, or imported-texture cache root "
	    "to derive a cooked texture output path: '"
	    + request.sourcePath.string() + "'.");
}

static std::string BuildTextureVariantSuffix(const TextureCookRequest& request)
{
	return "." + Formatting::FormatHexUInt64(request.assetId);
}
