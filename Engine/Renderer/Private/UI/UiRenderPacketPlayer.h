#pragma once

#include "Renderer/Public/UI/UiTextureHandle.h"

#include "Renderer/Public/UI/UiRenderPacket.h"

#include <cstddef>
#include <memory>

class RenderCommandList;
class RhiImGuiRenderer;
struct ImTextureData;

class UiRenderPacketPlayer final
{
public:
	UiRenderPacketPlayer();
	~UiRenderPacketPlayer() noexcept;

	UiRenderPacketPlayer(const UiRenderPacketPlayer&) = delete;
	UiRenderPacketPlayer& operator=(const UiRenderPacketPlayer&) = delete;

	void SynchronizeTextures(const UiRenderPacket& packet, RhiImGuiRenderer& renderer);
	void Render(const UiRenderPacket& packet, UiTextureHandle viewportTexture, std::uint64_t viewportTextureId, RhiImGuiRenderer& renderer, RenderCommandList& commandList);
	void Shutdown(RhiImGuiRenderer& renderer) noexcept;

private:
	struct PlaybackStorage;

	void ApplyTextureChanges(const UiRenderPacket& packet);
	void ApplyTextureUpload(const UiRenderPacket& packet, const UiTextureUpload& upload);
	void QueueTextureRelease(UiTextureHandle handle) noexcept;
	ImTextureData* FindTexture(UiTextureHandle handle) const noexcept;
	void RetireReleasedTextures() noexcept;
	void PrepareDrawLists(std::size_t drawListCount);
	void CopyDrawList(const UiRenderPacket& packet, const UiDrawList& packetList, std::size_t drawListIndex, UiTextureHandle viewportTexture, std::uint64_t viewportTextureId);
	void PrepareDrawData(const UiRenderPacket& packet);

	std::unique_ptr<PlaybackStorage> m_storage;
};
