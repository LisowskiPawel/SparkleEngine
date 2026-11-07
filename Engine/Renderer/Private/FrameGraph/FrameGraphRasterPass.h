#pragma once

#include "Pipeline/GraphicsPipelineMaterialization.h"
#include "FrameGraph/FrameGraphAttachment.h"

#include <array>
#include <cstdint>

struct FrameGraphRasterPass final
{
	std::array<FrameGraphAttachmentBinding, 8> Colors = {};
	std::uint32_t ColorCount = 0;
	FrameGraphAttachmentBinding DepthStencil = {};
	bool bHasDepthStencil = false;
	GraphicsAttachmentSignature Compatibility = {};
};
