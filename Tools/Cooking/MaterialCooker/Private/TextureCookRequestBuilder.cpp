#include "PCH.h"
#include "Core/Public/Paths/CookedAssetPaths.h"

#include "TextureCookRequestBuilder.h"
#include "MaterialCooker.h"
#include "SourceImportOutput.h"

#include "SourceAssetPaths.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Paths/PathUtils.h"

#include <string>
#include <string_view>

struct TextureSourceLocation final
{
	std::string_view rootName;
	std::string_view cookedDirectory;
	std::filesystem::path relativePath;
};

static TextureColorSpace ResolveColorSpace(TextureGroup textureGroup, const std::filesystem::path& sourcePath);
static TextureMipPolicy ResolveMipPolicy(const std::filesystem::path& sourceTexturePath);
static std::filesystem::path NormalizeSourceTexturePath(const std::filesystem::path& sourceTexturePath);
static TextureSourceLocation ResolveSourceLocation(const std::filesystem::path& sourcePath);

static std::string BuildTextureSourceKey(const TextureCookRequest& request, const TextureSourceLocation& location)
{
	return std::string(location.rootName) + ":" + GetTextureColorSpaceName(request.policy.colorSpace) + ":" + GetTextureMipPolicyName(request.policy.mipPolicy) + ":"
	    + GetTextureColorProcessingPolicyName(request.policy.colorProcessingPolicy) + ":" + GetTextureGroupName(request.policy.textureGroup) + ":" + GetTextureDimensionName(request.policy.dimension)
	    + ":" + GetTextureChannelMaskName(request.policy.channelMask) + ":" + GetTextureHdrEncodingName(request.policy.hdrEncoding) + ":" + location.relativePath.generic_string();
}

static std::filesystem::path BuildTextureOutputPath(const TextureCookRequest& request, const TextureSourceLocation& location)
{
	std::filesystem::path cookedRelativePath = std::filesystem::path(location.cookedDirectory) / location.relativePath;
	cookedRelativePath.replace_extension("." + Formatting::FormatHexUInt64(request.assetId) + ".stex");
	return Filesystem::GetCookedTextureRootPath() / cookedRelativePath;
}

TextureCookRequest BuildTextureCookRequest(const std::filesystem::path& sourceTexturePath, TextureGroup textureGroup, TextureChannelMask channelMask)
{
	TextureCookRequest request;
	request.sourcePath = NormalizeSourceTexturePath(sourceTexturePath);
	request.policy.colorSpace = ResolveColorSpace(textureGroup, request.sourcePath);
	request.policy.mipPolicy = ResolveMipPolicy(request.sourcePath);
	request.policy.colorProcessingPolicy = request.IsSrgb() ? TextureColorProcessingPolicy::SrgbLinearize : TextureColorProcessingPolicy::Linear;
	request.policy.textureGroup = textureGroup;
	request.policy.channelMask = channelMask;
	const TextureSourceLocation location = ResolveSourceLocation(request.sourcePath);
	const std::string sourceKey = BuildTextureSourceKey(request, location);
	request.assetId = Hash::Fnv1a64(sourceKey);
	request.outputPath = BuildTextureOutputPath(request, location);
	return request;
}

void MaterialCooker::AppendTextureCookRequests(const SourceImportOutput& importOutput, TextureCookRequestSet& requests)
{
	for (const ImportedMaterial& material : importOutput.scene.materials)
	{
		for (const ImportedTextureSource& textureSource : material.textureSources)
		{
			if (textureSource.sourcePath.empty())
			{
				continue;
			}

			const TextureCookRequest request = BuildTextureCookRequest(textureSource.sourcePath, textureSource.textureGroup, textureSource.channelMask);
			requests.Add(request);
		}
	}
}

static TextureColorSpace ResolveColorSpace(TextureGroup textureGroup, const std::filesystem::path& sourcePath)
{
	switch (textureGroup)
	{
		case TextureGroup::Diffuse:
		case TextureGroup::Emissive:
		case TextureGroup::SubsurfaceColor:
		{
			const auto extension = Paths::GetLowercaseExtension(sourcePath);
			const bool bRadiance = extension == L".hdr" || extension == L".hdri";
			const bool bLinearHdr = extension == L".exr" || bRadiance;
			return bLinearHdr ? TextureColorSpace::Linear : TextureColorSpace::Srgb;
		}

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

static TextureMipPolicy ResolveMipPolicy(const std::filesystem::path& sourceTexturePath)
{
	const auto extension = Paths::GetLowercaseExtension(sourceTexturePath);
	const bool bKtx = extension == L".ktx" || extension == L".ktx2";
	if (extension == L".dds" || bKtx)
	{
		return TextureMipPolicy::PreserveExisting;
	}
	return TextureMipPolicy::Generate;
}

static std::filesystem::path NormalizeSourceTexturePath(const std::filesystem::path& sourceTexturePath)
{
	const auto resolvedPath = SparkleTools::ResolveSourceAssetPath(sourceTexturePath, SparkleTools::SourceAssetType::Texture);

	if (resolvedPath)
	{
		return *resolvedPath;
	}

	throw Diagnostics::Error("Unable to resolve source texture path '" + sourceTexturePath.string() + "'.");
}

static TextureSourceLocation ResolveSourceLocation(const std::filesystem::path& sourcePath)
{
	const std::filesystem::path& projectRoot = Filesystem::GetProjectPath();
	if (const auto relativePath = Paths::TryMakeRelativeUnderRoot(sourcePath, projectRoot))
	{
		return {"project", "Project", *relativePath};
	}

	const std::filesystem::path& engineRoot = Filesystem::GetEnginePath();
	if (const auto relativePath = Paths::TryMakeRelativeUnderRoot(sourcePath, engineRoot))
	{
		return {"engine", "Engine", *relativePath};
	}

	const std::filesystem::path importedTextureRoot = SparkleTools::ImportedTextureCacheRoot();
	if (const auto relativePath = Paths::TryMakeRelativeUnderRoot(sourcePath, importedTextureRoot))
	{
		return {"imported", "Imported", *relativePath};
	}

	throw Diagnostics::Error(
	    "Source texture path must be under the project, engine, or imported-texture cache root "
	    "to derive its cooked texture identity and output path: '"
	    + sourcePath.string() + "'.");
}
