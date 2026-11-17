#include "PCH.h"

#include "Resources/RhiResourceView.h"

RhiResourceViewDesc RhiResourceViewDesc::TextureShaderResource(RhiResourceHandle resource, PixelFormat format, RhiTextureViewRange range, TextureResourceDimension dimension) noexcept
{
	return RhiResourceViewDesc{.Kind = ERhiResourceViewKind::TextureShaderResource, .Resource = resource, .Format = format, .Texture = range, .TextureDimension = dimension};
}

RhiResourceViewDesc RhiResourceViewDesc::TextureUnorderedAccess(RhiResourceHandle resource, PixelFormat format) noexcept
{
	return RhiResourceViewDesc{.Kind = ERhiResourceViewKind::TextureUnorderedAccess, .Resource = resource, .Format = format};
}

RhiResourceViewDesc RhiResourceViewDesc::RenderTarget(RhiResourceHandle resource, PixelFormat format) noexcept
{
	return RhiResourceViewDesc{.Kind = ERhiResourceViewKind::RenderTarget, .Resource = resource, .Format = format};
}

RhiResourceViewDesc RhiResourceViewDesc::DepthStencil(RhiResourceHandle resource, PixelFormat format) noexcept
{
	return RhiResourceViewDesc{.Kind = ERhiResourceViewKind::DepthStencil, .Resource = resource, .Format = format};
}

RhiResourceViewDesc RhiResourceViewDesc::BufferShaderResource(RhiResourceHandle resource, std::uint64_t sizeInBytes, std::uint32_t strideInBytes, std::uint64_t offsetInBytes) noexcept
{
	return RhiResourceViewDesc{
	    .Kind = ERhiResourceViewKind::BufferShaderResource,
	    .Resource = resource,
	    .Buffer = RhiBufferViewRange{.OffsetInBytes = offsetInBytes, .SizeInBytes = sizeInBytes, .StrideInBytes = strideInBytes}};
}

RhiResourceViewDesc RhiResourceViewDesc::BufferUnorderedAccess(RhiResourceHandle resource, std::uint64_t sizeInBytes, std::uint32_t strideInBytes, std::uint64_t offsetInBytes) noexcept
{
	return RhiResourceViewDesc{
	    .Kind = ERhiResourceViewKind::BufferUnorderedAccess,
	    .Resource = resource,
	    .Buffer = RhiBufferViewRange{.OffsetInBytes = offsetInBytes, .SizeInBytes = sizeInBytes, .StrideInBytes = strideInBytes}};
}
