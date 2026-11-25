#include "Cooking/DefaultTextureCookRequestBuilder.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Assets/DefaultTexture.h"
#include "Core/Public/Paths/CookedAssetPaths.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Hash/HashUtils.h"
#include "TextureCookRequestList.h"

#include <array>
#include <string>
#include <string_view>
#include <system_error>

struct DefaultTextureCookDesc final
{
	std::string_view SourceRelativePath;
	DefaultTexture Product;
	TextureColorSpace ColorSpace = TextureColorSpace::Srgb;
	TextureColorProcessingPolicy ColorProcessingPolicy = TextureColorProcessingPolicy::SrgbLinearize;
	TextureGroup Group = TextureGroup::Diffuse;
};

static constexpr std::array DefaultTextures = {
    DefaultTextureCookDesc{"Assets/Textures/Defaults/default_checkerboard.png", DefaultTexture::Checkerboard},
    DefaultTextureCookDesc{"Assets/Textures/Defaults/default_white.png", DefaultTexture::White},
    DefaultTextureCookDesc{"Assets/Textures/Defaults/default_black.png", DefaultTexture::Black},
    DefaultTextureCookDesc{"Assets/Textures/Defaults/default_red.png", DefaultTexture::Red},
    DefaultTextureCookDesc{"Assets/Textures/Defaults/default_green.png", DefaultTexture::Green},
    DefaultTextureCookDesc{"Assets/Textures/Defaults/default_blue.png", DefaultTexture::Blue},
    DefaultTextureCookDesc{"Assets/Textures/Defaults/default_normal.png", DefaultTexture::Normal, TextureColorSpace::Linear, TextureColorProcessingPolicy::Linear, TextureGroup::NormalMap},
    DefaultTextureCookDesc{"Assets/Textures/Sky/evening_road_01_puresky_4k.exr", DefaultTexture::Sky, TextureColorSpace::Linear, TextureColorProcessingPolicy::Linear, TextureGroup::HdrColor}};

static TextureCookRequest BuildRequest(const DefaultTextureCookDesc& description)
{
	const std::filesystem::path sourcePath = (Filesystem::GetEnginePath() / std::filesystem::path(description.SourceRelativePath)).lexically_normal();
	std::error_code errorCode;
	if (!std::filesystem::exists(sourcePath, errorCode))
	{
		throw Diagnostics::Error("Default source texture was not found: " + sourcePath.string() + ".");
	}

	const DefaultTextureDesc& product = DefaultTextureDescs[static_cast<std::size_t>(description.Product)];
	TextureCookRequest request;
	request.assetId = Hash::Fnv1a64(std::string("engine-default-texture:") + std::string(product.path));
	request.sourcePath = sourcePath;
	request.outputPath = (Filesystem::GetCookedTextureRootPath() / std::filesystem::path(product.path)).lexically_normal();
	request.policy.colorSpace = description.ColorSpace;
	request.policy.mipPolicy = TextureMipPolicy::Generate;
	request.policy.colorProcessingPolicy = description.ColorProcessingPolicy;
	request.policy.textureGroup = description.Group;
	request.policy.dimension = TextureDimension::Texture2D;
	request.policy.channelMask = TextureChannelMask::Rgba;
	return request;
}

void AppendDefaultTextureCookRequests(TextureCookRequestSet& requestSet)
{
	for (const DefaultTextureCookDesc& description : DefaultTextures)
	{
		const TextureCookRequest request = BuildRequest(description);
		requestSet.Add(request);
	}
}
