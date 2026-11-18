#include "TextureCookRequestCodec.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Core/Public/Diagnostics/Verify.h"

#include <sstream>
#include <stdexcept>
#include <vector>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_textureCookRequestCodecLogger, "TextureCooker.RequestCodec");

namespace TextureCookRequestEncoding
{
	template <typename T> struct PolicyName final
	{
		T Value;
		const char* Name;
	};

	static constexpr PolicyName<TextureColorSpace> ColorSpaces[] = {{TextureColorSpace::Linear, "linear"}, {TextureColorSpace::Srgb, "srgb"}};

	static constexpr PolicyName<TextureMipPolicy> MipPolicies[] = {
	    {TextureMipPolicy::Generate, "generate"},
	    {TextureMipPolicy::PreserveExisting, "preserve-existing"},
	    {TextureMipPolicy::NoMips, "no-mips"}};

	static constexpr PolicyName<TextureColorProcessingPolicy> ColorProcessingPolicies[] = {
	    {TextureColorProcessingPolicy::Linear, "linear"},
	    {TextureColorProcessingPolicy::SrgbLinearize, "srgb-linearize"}};

	static constexpr PolicyName<TextureGroup> Groups[] = {
	    {TextureGroup::Default, "default"},
	    {TextureGroup::Diffuse, "diffuse"},
	    {TextureGroup::NormalMap, "normal-map"},
	    {TextureGroup::Roughness, "roughness"},
	    {TextureGroup::Metallic, "metallic"},
	    {TextureGroup::AmbientOcclusion, "ambient-occlusion"},
	    {TextureGroup::Emissive, "emissive"},
	    {TextureGroup::SubsurfaceColor, "subsurface-color"},
	    {TextureGroup::SubsurfaceStrength, "subsurface-strength"},
	    {TextureGroup::HdrColor, "hdr-color"}};

	static constexpr PolicyName<TextureDimension> Dimensions[] = {{TextureDimension::Texture2D, "2d"}, {TextureDimension::TextureCube, "cube"}};

	static constexpr PolicyName<TextureChannelMask> Channels[] =
	    {{TextureChannelMask::Rgba, "rgba"}, {TextureChannelMask::Red, "red"}, {TextureChannelMask::Green, "green"}, {TextureChannelMask::Blue, "blue"}, {TextureChannelMask::Alpha, "alpha"}};

	static constexpr PolicyName<TextureHdrEncoding> HdrEncodings[] = {{TextureHdrEncoding::Rgba32Float, "rgba32f"}, {TextureHdrEncoding::Bc6h, "bc6h"}};

	template <typename T, std::size_t N> static T Parse(std::string_view value, const PolicyName<T> (&names)[N])
	{
		for (const auto& entry : names)
		{
			if (Strings::EqualsIgnoreCase(value, entry.Name))
			{
				return entry.Value;
			}
		}
		throw Diagnostics::Error("Unknown texture request policy '" + std::string(value) + "'.");
	}

	template <typename T, std::size_t N> static const char* Name(T value, const PolicyName<T> (&names)[N]) noexcept
	{
		for (const auto& entry : names)
		{
			if (value == entry.Value)
			{
				return entry.Name;
			}
		}
		Diagnostics::Fatal(g_textureCookRequestCodecLogger, __FILE__, __LINE__, "Unknown texture request policy value.");
	}

	static constexpr std::string_view Header = "TextureCookRequests|1";

	static constexpr std::size_t FieldCount = 10;

	static TextureAssetId ParseAssetId(std::string_view value)
	{
		std::uint64_t parsedAssetId = 0;
		if (!Formatting::TryParseHexUInt64(value, parsedAssetId) || parsedAssetId == InvalidTextureAssetId)
		{
			throw Diagnostics::Error("Texture cook request entry has an invalid asset id '" + std::string(value) + "'.");
		}
		return static_cast<TextureAssetId>(parsedAssetId);
	}

	static std::filesystem::path NormalizePath(std::string_view value)
	{
		try
		{
			return std::filesystem::path(Strings::ToWide(value)).lexically_normal();
		}
		catch (const std::invalid_argument&)
		{
			throw Diagnostics::Error("Texture cook request path is not valid UTF-8.");
		}
	}

	static std::string FormatPath(const std::filesystem::path& path)
	{
		const std::string formatted = Strings::ToNarrow(path.generic_wstring());
		if (formatted.find_first_of(std::string_view("|\r\n\0", 4)) != std::string::npos)
		{
			throw Diagnostics::Error("Texture cook request paths cannot contain pipe, CR, LF or NUL characters.");
		}
		return formatted;
	}
}

std::string_view TextureCookRequestCodec::GetHeader() noexcept
{
	return TextureCookRequestEncoding::Header;
}

bool TextureCookRequestCodec::IsHeader(std::string_view line) noexcept
{
	return line == TextureCookRequestEncoding::Header;
}

TextureCookRequest TextureCookRequestCodec::ParseLine(std::string_view line)
{
	const std::vector<std::string_view> fields = Strings::Split(line, '|');
	if (fields.size() != TextureCookRequestEncoding::FieldCount)
	{
		throw Diagnostics::Error("Texture cook request entry is malformed.");
	}
	if (fields[8].empty())
	{
		throw Diagnostics::Error("Texture cook request entry is missing an output path.");
	}
	if (fields[9].empty())
	{
		throw Diagnostics::Error("Texture cook request entry is missing a source path.");
	}

	TextureCookRequest request;
	request.assetId = TextureCookRequestEncoding::ParseAssetId(fields[0]);
	request.policy.colorSpace = TextureCookRequestEncoding::Parse(fields[1], TextureCookRequestEncoding::ColorSpaces);
	request.policy.mipPolicy = TextureCookRequestEncoding::Parse(fields[2], TextureCookRequestEncoding::MipPolicies);
	request.policy.colorProcessingPolicy = TextureCookRequestEncoding::Parse(fields[3], TextureCookRequestEncoding::ColorProcessingPolicies);
	request.policy.textureGroup = TextureCookRequestEncoding::Parse(fields[4], TextureCookRequestEncoding::Groups);
	request.policy.dimension = TextureCookRequestEncoding::Parse(fields[5], TextureCookRequestEncoding::Dimensions);
	request.policy.channelMask = TextureCookRequestEncoding::Parse(fields[6], TextureCookRequestEncoding::Channels);
	request.policy.hdrEncoding = TextureCookRequestEncoding::Parse(fields[7], TextureCookRequestEncoding::HdrEncodings);
	request.outputPath = TextureCookRequestEncoding::NormalizePath(fields[8]);
	request.sourcePath = TextureCookRequestEncoding::NormalizePath(fields[9]);
	return request;
}

std::string TextureCookRequestCodec::FormatLine(const TextureCookRequest& request)
{
	std::ostringstream output;
	output << Formatting::FormatHexUInt64(request.assetId) << '|' << GetTextureColorSpaceName(request.policy.colorSpace) << '|' << GetTextureMipPolicyName(request.policy.mipPolicy) << '|'
	       << GetTextureColorProcessingPolicyName(request.policy.colorProcessingPolicy) << '|' << GetTextureGroupName(request.policy.textureGroup) << '|'
	       << GetTextureDimensionName(request.policy.dimension) << '|' << GetTextureChannelMaskName(request.policy.channelMask) << '|' << GetTextureHdrEncodingName(request.policy.hdrEncoding) << '|'
	       << TextureCookRequestEncoding::FormatPath(request.outputPath) << '|' << TextureCookRequestEncoding::FormatPath(request.sourcePath);
	return output.str();
}

const char* GetTextureColorSpaceName(TextureColorSpace value) noexcept
{
	return TextureCookRequestEncoding::Name(value, TextureCookRequestEncoding::ColorSpaces);
}

const char* GetTextureMipPolicyName(TextureMipPolicy value) noexcept
{
	return TextureCookRequestEncoding::Name(value, TextureCookRequestEncoding::MipPolicies);
}

const char* GetTextureColorProcessingPolicyName(TextureColorProcessingPolicy value) noexcept
{
	return TextureCookRequestEncoding::Name(value, TextureCookRequestEncoding::ColorProcessingPolicies);
}

const char* GetTextureGroupName(TextureGroup value) noexcept
{
	return TextureCookRequestEncoding::Name(value, TextureCookRequestEncoding::Groups);
}

const char* GetTextureDimensionName(TextureDimension value) noexcept
{
	return TextureCookRequestEncoding::Name(value, TextureCookRequestEncoding::Dimensions);
}

const char* GetTextureChannelMaskName(TextureChannelMask value) noexcept
{
	return TextureCookRequestEncoding::Name(value, TextureCookRequestEncoding::Channels);
}

const char* GetTextureHdrEncodingName(TextureHdrEncoding value) noexcept
{
	return TextureCookRequestEncoding::Name(value, TextureCookRequestEncoding::HdrEncodings);
}
