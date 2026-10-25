#pragma once

#include "UI/RhiImGuiRenderer.h"
#include "Vulkan/VulkanIncludes.h"

#include <cstdint>
#include <vector>

struct ImDrawData;
struct ImGuiContext;
class VulkanDescriptorService;
class VulkanRhi;
class VulkanSwapChain;

class VulkanImGuiBackend final : public RhiImGuiRenderer
{
public:
	VulkanImGuiBackend(VulkanRhi& rhi, VulkanSwapChain& swapChain, VulkanDescriptorService& descriptors) noexcept;

	void Initialize() override;
	void BeginFrame() noexcept override;
	std::uint64_t ResolveTextureId(RhiGpuDescriptorHandle shaderResourceView) noexcept override;
	void UpdateTexture(ImTextureData& texture) noexcept override;
	void RenderDrawData(RenderCommandList& commandList, ImDrawData* drawData) noexcept override;
	void Shutdown() noexcept override;

private:
	void InitializeRendererBackend(ImGuiContext* previousContext);
	void CreateTextureSampler(ImGuiContext* previousContext);

	struct TextureBinding final
	{
		VkImageView ImageView = VK_NULL_HANDLE;
		VkDescriptorSet DescriptorSet = VK_NULL_HANDLE;
	};

	std::uint64_t GetTextureId(VkImageView imageView) noexcept;

	VulkanRhi& m_rhi;
	VulkanSwapChain& m_swapChain;
	VulkanDescriptorService& m_descriptorService;
	VkSampler m_imguiSampler = VK_NULL_HANDLE;
	std::vector<TextureBinding> m_textureBindings;
	ImGuiContext* m_imguiContext = nullptr;
	bool m_ownsContext = false;
};
