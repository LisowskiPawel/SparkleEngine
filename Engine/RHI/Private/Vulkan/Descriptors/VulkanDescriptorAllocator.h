#pragma once

#include "Descriptors/RhiDescriptorHandles.h"
#include "Frame/RhiFrameConstants.h"
#include "Resources/RhiResourceView.h"
#include "Samplers/RhiSamplerDesc.h"
#include "Vulkan/Descriptors/VulkanDescriptorWrites.h"

#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <span>
#include <vector>

class VulkanRhi;
class VulkanDescriptorService;
struct CompiledBinding;

class VulkanDescriptorAllocator final
{
public:
	explicit VulkanDescriptorAllocator(VulkanRhi& rhi) noexcept;
	~VulkanDescriptorAllocator() noexcept = default;

	VulkanDescriptorAllocator(const VulkanDescriptorAllocator&) = delete;
	VulkanDescriptorAllocator& operator=(const VulkanDescriptorAllocator&) = delete;
	VulkanDescriptorAllocator(VulkanDescriptorAllocator&&) = delete;
	VulkanDescriptorAllocator& operator=(VulkanDescriptorAllocator&&) = delete;

	RhiDescriptorAllocation AllocateDescriptor(ERhiDescriptorAllocatorType descriptorType);
	void BeginFrame(std::uint32_t frameIndex) noexcept;
	void ReleaseDescriptor(ERhiDescriptorAllocatorType descriptorType, const RhiDescriptorAllocation& allocation) noexcept;
	RhiDescriptorTableHandle AllocateDescriptorTable(ERhiDescriptorAllocatorType descriptorType, std::uint32_t descriptorCount);
	RhiCpuDescriptorHandle GetDescriptorTableCpuHandle(RhiDescriptorTableHandle tableHandle, std::uint32_t descriptorIndex = 0) const noexcept;
	void ReleaseDescriptorTable(RhiDescriptorTableHandle tableHandle) noexcept;

	RhiGpuDescriptorHandle RegisterImageDescriptor(ERhiResourceViewKind viewKind, VkImageView imageView);
	RhiGpuDescriptorHandle RegisterBufferDescriptor(ERhiResourceViewKind viewKind, VkBuffer buffer, std::uint64_t offsetInBytes, std::uint64_t sizeInBytes);
	void ReleaseRegisteredDescriptor(RhiGpuDescriptorHandle handle) noexcept;
	void WriteSamplerDescriptor(RhiCpuDescriptorHandle destination, VkSampler sampler) noexcept;
	bool WriteRegisteredDescriptor(RhiCpuDescriptorHandle destination, RhiGpuDescriptorHandle source) noexcept;

	void WriteDescriptorTable(VkDescriptorSet descriptorSet, const CompiledBinding& binding, RhiDescriptorTableBinding tableBinding) noexcept;
	void WriteDescriptorHandle(VkDescriptorSet descriptorSet, const CompiledBinding& binding, RhiGpuDescriptorHandle handle) noexcept;
	void WriteBufferDescriptor(VkDescriptorSet descriptorSet, const CompiledBinding& binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) noexcept;
	void WriteAccelerationStructureDescriptor(VkDescriptorSet descriptorSet, const CompiledBinding& binding, VkAccelerationStructureKHR accelerationStructure) noexcept;
	void WritePtlasDescriptor(VkDescriptorSet descriptorSet, const CompiledBinding& binding, VkDeviceAddress accelerationStructureAddress) noexcept;

private:
	friend class VulkanDescriptorService;

	struct DescriptorTableRecord final
	{
		ERhiDescriptorAllocatorType Type = ERhiDescriptorAllocatorType::ShaderResource;
		std::shared_ptr<std::vector<VulkanDescriptorEntry>> Entries;
		bool bAllocated = false;
		std::uint16_t Generation = 0;
	};

	struct DescriptorTableReadRecord final
	{
		std::shared_ptr<const std::vector<VulkanDescriptorEntry>> Entries;
		bool bAllocated = false;
		std::uint16_t Generation = 0;
	};

	struct RecordingReadView final
	{
		std::vector<DescriptorTableReadRecord> Tables;
		std::shared_ptr<const std::vector<VulkanDescriptorEntry>> RegisteredDescriptors;
	};

	static VulkanDescriptorEntryKind ToImageEntryKind(ERhiResourceViewKind viewKind) noexcept;
	static VulkanDescriptorEntryKind ToBufferEntryKind(ERhiResourceViewKind viewKind) noexcept;

	DescriptorTableRecord* FindTableRecord(RhiDescriptorTableHandle tableHandle) noexcept;
	const DescriptorTableRecord* FindTableRecord(RhiDescriptorTableHandle tableHandle) const noexcept;
	std::vector<VulkanDescriptorEntry>& EditTableEntries(DescriptorTableRecord& record);
	std::vector<VulkanDescriptorEntry>& EditRegisteredDescriptors();
	void PublishRecordingReadView() noexcept;
	void RecycleTableRecord(std::uint32_t tableIndex) noexcept;
	VulkanDescriptorEntry* FindRegisteredEntry(RhiGpuDescriptorHandle handle) noexcept;
	const VulkanDescriptorEntry* FindRegisteredEntry(RhiGpuDescriptorHandle handle) const noexcept;
	std::shared_ptr<const RecordingReadView> GetRecordingReadView() const noexcept;

	VulkanRhi& m_rhi;
	mutable std::mutex m_registryMutex;
	std::uint32_t m_currentFrameIndex = 0;
	std::vector<DescriptorTableRecord> m_tables;
	std::vector<std::uint32_t> m_freeTableIndices;
	std::array<std::vector<std::uint32_t>, RhiFrameConstants::MaxFrameSlotCount> m_retiredTableIndices;
	std::shared_ptr<std::vector<VulkanDescriptorEntry>> m_registeredDescriptors;
	std::atomic<std::shared_ptr<const RecordingReadView>> m_recordingReadView;
	std::vector<std::uint32_t> m_freeRegisteredDescriptorIndices;
};
