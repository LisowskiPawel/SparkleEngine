#pragma once

#include <cstdint>
#include <optional>

class FrameGraph;
class RendererImageProviderStack;
struct FrameHistoryResourceLayout;
struct RenderFrame;

class RestirLightingHistory final
{
public:
	void Update(FrameGraph& frameGraph, const FrameHistoryResourceLayout& history, const RenderFrame& frame, RendererImageProviderStack& imageProviders);

private:
	std::optional<std::uint64_t> m_invalidationHash;
};
