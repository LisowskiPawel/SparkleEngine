#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Descriptors/VulkanDescriptorAllocator.h"

#include "Pipeline/RhiPipelineDesc.h"
#include "Vulkan/Core/VulkanResult.h"
#include "Vulkan/Descriptors/VulkanDescriptorHandles.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Core/Public/Diagnostics/Verify.h"

#include <algorithm>
#include <utility>
#include <array>
#include <format>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_vulkanDescriptorAllocatorLogger, "RHI.Vulkan.DescriptorAllocator");

VulkanDescriptorAllocator::VulkanDescriptorAllocator(VulkanRhi& rhi) noexcept :
    m_rhi(rhi),
    m_registeredDescriptors(std::make_shared<std::vector<VulkanDescriptorEntry>>())
{
	PublishRecordingReadView();
}

RhiDescriptorAllocation VulkanDescriptorAllocator::AllocateDescriptor(ERhiDescriptorAllocatorType descriptorType)
{
	const RhiDescriptorTableHandle table = AllocateDescriptorTable(descriptorType, 1);
	return RhiDescriptorAllocation{.CpuHandle = GetDescriptorTableCpuHandle(table), .GpuHandle = {}};
}

void VulkanDescriptorAllocator::BeginFrame(std::uint32_t frameIndex) noexcept
{
	if (frameIndex >= m_retiredTableIndices.size())
	{
		return;
	}

	{
		std::scoped_lock lock(m_registryMutex);
		for (const std::uint32_t tableIndex : m_retiredTableIndices[frameIndex])
		{
			RecycleTableRecord(tableIndex);
		}
		m_retiredTableIndices[frameIndex].clear();
		m_currentFrameIndex = frameIndex;
	}
}

void VulkanDescriptorAllocator::PublishRecordingReadView() noexcept
{
	auto readView = std::make_shared<RecordingReadView>();
	{
		std::scoped_lock lock(m_registryMutex);
		readView->Tables.reserve(m_tables.size());
		for (const DescriptorTableRecord& table : m_tables)
		{
			readView->Tables.push_back(DescriptorTableReadRecord{.Entries = table.Entries, .bAllocated = table.bAllocated, .Generation = table.Generation});
		}
		readView->RegisteredDescriptors = m_registeredDescriptors;
	}

	m_recordingReadView.store(std::shared_ptr<const RecordingReadView>(std::move(readView)), std::memory_order_release);
}

void VulkanDescriptorAllocator::ReleaseDescriptor(ERhiDescriptorAllocatorType, const RhiDescriptorAllocation& allocation) noexcept
{
	RhiDescriptorTableHandle tableHandle{};
	std::uint32_t descriptorIndex = 0;
	const bool bAllocationDecoded = VulkanDescriptorHandles::DecodeCpuDescriptorHandle(allocation.CpuHandle, tableHandle, descriptorIndex);
	const bool bAllocationStartsAtZero = bAllocationDecoded && descriptorIndex == 0;

	if (!bAllocationStartsAtZero)
	{
		return;
	}
	ReleaseDescriptorTable(tableHandle);
}

RhiDescriptorTableHandle VulkanDescriptorAllocator::AllocateDescriptorTable(ERhiDescriptorAllocatorType descriptorType, std::uint32_t descriptorCount)
{
	if (descriptorCount == 0 || descriptorCount > VulkanDescriptorHandles::MaximumCpuDescriptorCount)
	{
		return {};
	}

	std::scoped_lock lock(m_registryMutex);
	if (m_freeTableIndices.empty() && m_tables.size() >= RhiDescriptorTableHandle::MaximumRecordCount)
	{
		return {};
	}

	DescriptorTableRecord record{};
	record.Type = descriptorType;
	record.Entries = std::make_shared<std::vector<VulkanDescriptorEntry>>(descriptorCount, VulkanDescriptorEntry{});
	record.bAllocated = true;

	if (!m_freeTableIndices.empty())
	{
		const std::uint32_t index = m_freeTableIndices.back();
		m_freeTableIndices.pop_back();
		record.Generation = m_tables[index].Generation;
		m_tables[index] = std::move(record);
		return VulkanDescriptorHandles::MakeTableHandle(index, m_tables[index].Generation);
	}

	m_tables.push_back(std::move(record));
	return VulkanDescriptorHandles::MakeTableHandle(static_cast<std::uint32_t>(m_tables.size() - 1), 0u);
}

RhiCpuDescriptorHandle VulkanDescriptorAllocator::GetDescriptorTableCpuHandle(RhiDescriptorTableHandle tableHandle, std::uint32_t descriptorIndex) const noexcept
{
	std::scoped_lock lock(m_registryMutex);
	const DescriptorTableRecord* const record = FindTableRecord(tableHandle);
	if (record == nullptr || record->Entries == nullptr || descriptorIndex >= record->Entries->size())
	{
		return {};
	}
	return VulkanDescriptorHandles::MakeCpuDescriptorHandle(tableHandle, descriptorIndex);
}

void VulkanDescriptorAllocator::ReleaseDescriptorTable(RhiDescriptorTableHandle tableHandle) noexcept
{
	std::scoped_lock lock(m_registryMutex);
	DescriptorTableRecord* const record = FindTableRecord(tableHandle);
	if (record == nullptr)
	{
		return;
	}

	std::uint32_t tableIndex = 0;
	std::uint16_t generation = 0;
	if (!tableHandle.Decode(tableIndex, generation))
	{
		return;
	}

	record->Entries.reset();
	record->bAllocated = false;
	m_retiredTableIndices[m_currentFrameIndex].push_back(tableIndex);
}

RhiGpuDescriptorHandle VulkanDescriptorAllocator::RegisterImageDescriptor(ERhiResourceViewKind viewKind, VkImageView imageView)
{
	if (imageView == VK_NULL_HANDLE)
	{
		return {};
	}

	VulkanDescriptorEntry entry{};
	entry.Kind = ToImageEntryKind(viewKind);
	entry.Image = VkDescriptorImageInfo{.sampler = VK_NULL_HANDLE, .imageView = imageView, .imageLayout = VulkanDescriptorWrites::ToImageLayout(entry.Kind)};

	std::scoped_lock lock(m_registryMutex);
	std::vector<VulkanDescriptorEntry>& descriptors = EditRegisteredDescriptors();
	if (!m_freeRegisteredDescriptorIndices.empty())
	{
		const std::uint32_t index = m_freeRegisteredDescriptorIndices.back();
		m_freeRegisteredDescriptorIndices.pop_back();
		descriptors[index] = entry;
		return VulkanDescriptorHandles::MakeGpuDescriptorHandle(index);
	}

	descriptors.push_back(entry);
	return VulkanDescriptorHandles::MakeGpuDescriptorHandle(static_cast<std::uint32_t>(descriptors.size() - 1));
}

RhiGpuDescriptorHandle VulkanDescriptorAllocator::RegisterBufferDescriptor(ERhiResourceViewKind viewKind, VkBuffer buffer, std::uint64_t offsetInBytes, std::uint64_t sizeInBytes)
{
	if (buffer == VK_NULL_HANDLE || sizeInBytes == 0)
	{
		return {};
	}

	VulkanDescriptorEntry entry{};
	entry.Kind = ToBufferEntryKind(viewKind);
	entry.Buffer = VkDescriptorBufferInfo{.buffer = buffer, .offset = offsetInBytes, .range = sizeInBytes};

	std::scoped_lock lock(m_registryMutex);
	std::vector<VulkanDescriptorEntry>& descriptors = EditRegisteredDescriptors();
	if (!m_freeRegisteredDescriptorIndices.empty())
	{
		const std::uint32_t index = m_freeRegisteredDescriptorIndices.back();
		m_freeRegisteredDescriptorIndices.pop_back();
		descriptors[index] = entry;
		return VulkanDescriptorHandles::MakeGpuDescriptorHandle(index);
	}

	descriptors.push_back(entry);
	return VulkanDescriptorHandles::MakeGpuDescriptorHandle(static_cast<std::uint32_t>(descriptors.size() - 1));
}

void VulkanDescriptorAllocator::ReleaseRegisteredDescriptor(RhiGpuDescriptorHandle handle) noexcept
{
	std::uint32_t index = 0;
	if (!VulkanDescriptorHandles::DecodeGpuDescriptorHandle(handle, index))
	{
		return;
	}

	std::scoped_lock lock(m_registryMutex);
	std::vector<VulkanDescriptorEntry>& descriptors = EditRegisteredDescriptors();
	if (index >= descriptors.size())
	{
		return;
	}
	if (descriptors[index].Kind == VulkanDescriptorEntryKind::Empty)
	{
		return;
	}
	descriptors[index] = VulkanDescriptorEntry{};
	m_freeRegisteredDescriptorIndices.push_back(index);
}

void VulkanDescriptorAllocator::WriteSamplerDescriptor(RhiCpuDescriptorHandle destination, VkSampler sampler) noexcept
{
	RhiDescriptorTableHandle tableHandle{};
	std::uint32_t descriptorIndex = 0;
	const bool bDestinationDecoded = VulkanDescriptorHandles::DecodeCpuDescriptorHandle(destination, tableHandle, descriptorIndex);
	const bool bSamplerAvailable = bDestinationDecoded && sampler != VK_NULL_HANDLE;

	if (!bSamplerAvailable)
	{
		Diagnostics::Fatal(g_vulkanDescriptorAllocatorLogger, __FILE__, __LINE__, "Vulkan sampler write requires a valid destination descriptor and sampler.");
	}

	std::scoped_lock lock(m_registryMutex);
	DescriptorTableRecord* const record = FindTableRecord(tableHandle);
	if (record == nullptr || record->Entries == nullptr || descriptorIndex >= record->Entries->size())
	{
		Diagnostics::Fatal(g_vulkanDescriptorAllocatorLogger, __FILE__, __LINE__, "Vulkan sampler write references an unavailable descriptor table entry.");
	}
	std::vector<VulkanDescriptorEntry>& entries = EditTableEntries(*record);
	VulkanDescriptorEntry& entry = entries[descriptorIndex];
	entry.Kind = VulkanDescriptorEntryKind::Sampler;
	entry.Image = VkDescriptorImageInfo{.sampler = sampler, .imageView = VK_NULL_HANDLE, .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED};
}

bool VulkanDescriptorAllocator::WriteRegisteredDescriptor(RhiCpuDescriptorHandle destination, RhiGpuDescriptorHandle source) noexcept
{
	RhiDescriptorTableHandle tableHandle{};
	std::uint32_t descriptorIndex = 0;
	if (!VulkanDescriptorHandles::DecodeCpuDescriptorHandle(destination, tableHandle, descriptorIndex))
	{
		return false;
	}

	std::scoped_lock lock(m_registryMutex);
	DescriptorTableRecord* const table = FindTableRecord(tableHandle);
	const VulkanDescriptorEntry* const sourceEntry = FindRegisteredEntry(source);
	const bool bInvalidDescriptorCopyEntries = table == nullptr || table->Entries == nullptr;
	const bool bInvalidDescriptorCopySourceEntry = bInvalidDescriptorCopyEntries || (sourceEntry == nullptr);
	const bool bInvalidDescriptorCopyKind = bInvalidDescriptorCopySourceEntry || (sourceEntry->Kind == VulkanDescriptorEntryKind::Empty);
	const bool bInvalidDescriptorCopy = bInvalidDescriptorCopyKind || (descriptorIndex >= table->Entries->size());

	if (bInvalidDescriptorCopy)
	{
		return false;
	}

	EditTableEntries(*table)[descriptorIndex] = *sourceEntry;
	return true;
}

void VulkanDescriptorAllocator::WriteDescriptorTable(VkDescriptorSet descriptorSet, const CompiledBinding& binding, RhiDescriptorTableBinding tableBinding) noexcept
{
	if (descriptorSet == VK_NULL_HANDLE || !tableBinding)
	{
		Diagnostics::Fatal(g_vulkanDescriptorAllocatorLogger, __FILE__, __LINE__, "Vulkan descriptor-table binding requires a descriptor set and table binding.");
	}

	const std::shared_ptr<const RecordingReadView> readView = GetRecordingReadView();
	std::uint32_t tableIndex = 0;
	std::uint16_t generation = 0;
	if (readView == nullptr || !tableBinding.Table.Decode(tableIndex, generation) || tableIndex >= readView->Tables.size())
	{
		Diagnostics::Fatal(g_vulkanDescriptorAllocatorLogger, __FILE__, __LINE__, "Vulkan descriptor-table binding cannot resolve its recording snapshot.");
	}

	const DescriptorTableReadRecord& table = readView->Tables[tableIndex];
	const bool bInvalidDescriptorTableGeneration = !table.bAllocated || table.Generation != generation;
	const bool bInvalidDescriptorTableEntries = bInvalidDescriptorTableGeneration || (table.Entries == nullptr);
	const bool bInvalidDescriptorTable = bInvalidDescriptorTableEntries || (tableBinding.DescriptorIndex >= table.Entries->size());

	if (bInvalidDescriptorTable)
	{
		Diagnostics::Fatal(
		    g_vulkanDescriptorAllocatorLogger,
		    __FILE__,
		    __LINE__,
		    std::format(
		        "Vulkan descriptor-table binding '{}' references stale or unavailable table {} generation {} "
		        "(snapshot allocated={}, generation={}, descriptor count={}, first descriptor={}).",
		        binding.Name != nullptr ? binding.Name : "<unnamed>",
		        tableIndex,
		        generation,
		        table.bAllocated,
		        table.Generation,
		        table.Entries != nullptr ? table.Entries->size() : 0u,
		        tableBinding.DescriptorIndex));
	}

	const std::size_t availableDescriptorCount = table.Entries->size() - tableBinding.DescriptorIndex;
	if (binding.DescriptorCount == 0 || availableDescriptorCount == 0 || (!binding.Bindless.bBindlessEligible && availableDescriptorCount < binding.DescriptorCount))
	{
		Diagnostics::Fatal(g_vulkanDescriptorAllocatorLogger, __FILE__, __LINE__, "Vulkan descriptor table does not contain the binding's complete descriptor range.");
	}
	const std::size_t descriptorCount = std::min<std::size_t>(availableDescriptorCount, binding.DescriptorCount);
	VulkanDescriptorWrites::Write(m_rhi.GetDevice(), descriptorSet, binding, std::span<const VulkanDescriptorEntry>(table.Entries->data() + tableBinding.DescriptorIndex, descriptorCount));
}

void VulkanDescriptorAllocator::WriteDescriptorHandle(VkDescriptorSet descriptorSet, const CompiledBinding& binding, RhiGpuDescriptorHandle handle) noexcept
{
	if (descriptorSet == VK_NULL_HANDLE || !handle)
	{
		Diagnostics::Fatal(g_vulkanDescriptorAllocatorLogger, __FILE__, __LINE__, "Vulkan descriptor-handle binding requires a descriptor set and registered handle.");
	}

	const std::shared_ptr<const RecordingReadView> readView = GetRecordingReadView();
	std::uint32_t descriptorIndex = 0;
	const bool bDescriptorViewAvailable = readView != nullptr && readView->RegisteredDescriptors != nullptr;
	const bool bDescriptorDecoded = bDescriptorViewAvailable && VulkanDescriptorHandles::DecodeGpuDescriptorHandle(handle, descriptorIndex);
	const bool bDescriptorInRange = bDescriptorDecoded && descriptorIndex < readView->RegisteredDescriptors->size();

	if (!bDescriptorInRange)
	{
		Diagnostics::Fatal(g_vulkanDescriptorAllocatorLogger, __FILE__, __LINE__, "Vulkan descriptor-handle binding cannot resolve its recording snapshot.");
	}

	const VulkanDescriptorEntry& entry = (*readView->RegisteredDescriptors)[descriptorIndex];
	if (entry.Kind == VulkanDescriptorEntryKind::Empty)
	{
		Diagnostics::Fatal(g_vulkanDescriptorAllocatorLogger, __FILE__, __LINE__, "Vulkan descriptor handle references a released entry.");
	}

	VulkanDescriptorWrites::Write(m_rhi.GetDevice(), descriptorSet, binding, std::span<const VulkanDescriptorEntry>(&entry, 1));
}

void VulkanDescriptorAllocator::WriteBufferDescriptor(VkDescriptorSet descriptorSet, const CompiledBinding& binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) noexcept
{
	if (descriptorSet == VK_NULL_HANDLE || buffer == VK_NULL_HANDLE || range == 0)
	{
		Diagnostics::Fatal(g_vulkanDescriptorAllocatorLogger, __FILE__, __LINE__, "Vulkan buffer binding requires a descriptor set, resolved buffer, and non-zero range.");
	}

	VulkanDescriptorEntry entry{};
	entry.Kind = binding.Type == CompiledBindingType::ConstantBuffer ? VulkanDescriptorEntryKind::UniformBuffer : VulkanDescriptorEntryKind::StorageBuffer;
	entry.Buffer = VkDescriptorBufferInfo{.buffer = buffer, .offset = offset, .range = range};
	VulkanDescriptorWrites::Write(m_rhi.GetDevice(), descriptorSet, binding, std::span<const VulkanDescriptorEntry>(&entry, 1));
}

void VulkanDescriptorAllocator::WriteAccelerationStructureDescriptor(VkDescriptorSet descriptorSet, const CompiledBinding& binding, VkAccelerationStructureKHR accelerationStructure) noexcept
{
	if (descriptorSet == VK_NULL_HANDLE || accelerationStructure == VK_NULL_HANDLE)
	{
		Diagnostics::Fatal(g_vulkanDescriptorAllocatorLogger, __FILE__, __LINE__, "Vulkan acceleration-structure binding requires a descriptor set and resolved acceleration structure.");
	}

	VulkanDescriptorEntry entry{};
	entry.Kind = VulkanDescriptorEntryKind::AccelerationStructure;
	entry.AccelerationStructure = accelerationStructure;
	VulkanDescriptorWrites::Write(m_rhi.GetDevice(), descriptorSet, binding, std::span<const VulkanDescriptorEntry>(&entry, 1));
}

void VulkanDescriptorAllocator::WritePtlasDescriptor(VkDescriptorSet descriptorSet, const CompiledBinding& binding, VkDeviceAddress accelerationStructureAddress) noexcept
{
	if (descriptorSet == VK_NULL_HANDLE || accelerationStructureAddress == 0)
	{
		Diagnostics::Fatal(g_vulkanDescriptorAllocatorLogger, __FILE__, __LINE__, "Vulkan PTLAS binding requires a descriptor set and GPU address.");
	}

	VulkanDescriptorEntry entry{};
	entry.Kind = VulkanDescriptorEntryKind::Ptlas;
	entry.PtlasAddress = accelerationStructureAddress;
	VulkanDescriptorWrites::Write(m_rhi.GetDevice(), descriptorSet, binding, std::span<const VulkanDescriptorEntry>(&entry, 1));
}

VulkanDescriptorEntryKind VulkanDescriptorAllocator::ToImageEntryKind(ERhiResourceViewKind viewKind) noexcept
{
	switch (viewKind)
	{
		case ERhiResourceViewKind::TextureUnorderedAccess:
			return VulkanDescriptorEntryKind::StorageImage;
		case ERhiResourceViewKind::TextureShaderResource:
			return VulkanDescriptorEntryKind::SampledImage;
		default:
			Diagnostics::Fatal(g_vulkanDescriptorAllocatorLogger, __FILE__, __LINE__, "Unsupported Vulkan image descriptor kind.");
	}
}

VulkanDescriptorEntryKind VulkanDescriptorAllocator::ToBufferEntryKind(ERhiResourceViewKind viewKind) noexcept
{
	switch (viewKind)
	{
		case ERhiResourceViewKind::BufferUnorderedAccess:
			return VulkanDescriptorEntryKind::StorageBuffer;
		case ERhiResourceViewKind::BufferShaderResource:
			return VulkanDescriptorEntryKind::StorageBuffer;
		default:
			Diagnostics::Fatal(g_vulkanDescriptorAllocatorLogger, __FILE__, __LINE__, "Unsupported Vulkan buffer descriptor kind.");
	}
}

VulkanDescriptorAllocator::DescriptorTableRecord* VulkanDescriptorAllocator::FindTableRecord(RhiDescriptorTableHandle tableHandle) noexcept
{
	return const_cast<DescriptorTableRecord*>(std::as_const(*this).FindTableRecord(tableHandle));
}

const VulkanDescriptorAllocator::DescriptorTableRecord* VulkanDescriptorAllocator::FindTableRecord(RhiDescriptorTableHandle tableHandle) const noexcept
{
	std::uint32_t tableIndex = 0;
	std::uint16_t generation = 0;
	if (!tableHandle.Decode(tableIndex, generation) || tableIndex >= m_tables.size())
	{
		return nullptr;
	}

	const DescriptorTableRecord& record = m_tables[tableIndex];
	return record.bAllocated && record.Generation == generation ? &record : nullptr;
}

std::vector<VulkanDescriptorEntry>& VulkanDescriptorAllocator::EditTableEntries(DescriptorTableRecord& record)
{
	if (record.Entries == nullptr)
	{
		record.Entries = std::make_shared<std::vector<VulkanDescriptorEntry>>();
	}
	else if (record.Entries.use_count() != 1)
	{
		record.Entries = std::make_shared<std::vector<VulkanDescriptorEntry>>(*record.Entries);
	}

	return *record.Entries;
}

std::vector<VulkanDescriptorEntry>& VulkanDescriptorAllocator::EditRegisteredDescriptors()
{
	if (m_registeredDescriptors == nullptr)
	{
		m_registeredDescriptors = std::make_shared<std::vector<VulkanDescriptorEntry>>();
	}
	else if (m_registeredDescriptors.use_count() != 1)
	{
		m_registeredDescriptors = std::make_shared<std::vector<VulkanDescriptorEntry>>(*m_registeredDescriptors);
	}

	return *m_registeredDescriptors;
}

void VulkanDescriptorAllocator::RecycleTableRecord(std::uint32_t tableIndex) noexcept
{
	if (tableIndex >= m_tables.size())
	{
		return;
	}

	DescriptorTableRecord& record = m_tables[tableIndex];
	if (record.bAllocated || record.Generation == RhiDescriptorTableHandle::MaximumGeneration)
	{
		return;
	}

	++record.Generation;
	m_freeTableIndices.push_back(tableIndex);
}

VulkanDescriptorEntry* VulkanDescriptorAllocator::FindRegisteredEntry(RhiGpuDescriptorHandle handle) noexcept
{
	return const_cast<VulkanDescriptorEntry*>(std::as_const(*this).FindRegisteredEntry(handle));
}

const VulkanDescriptorEntry* VulkanDescriptorAllocator::FindRegisteredEntry(RhiGpuDescriptorHandle handle) const noexcept
{
	if (m_registeredDescriptors == nullptr)
	{
		return nullptr;
	}

	std::uint32_t index = 0;
	if (!VulkanDescriptorHandles::DecodeGpuDescriptorHandle(handle, index) || index >= m_registeredDescriptors->size())
	{
		return nullptr;
	}
	return &(*m_registeredDescriptors)[index];
}

std::shared_ptr<const VulkanDescriptorAllocator::RecordingReadView> VulkanDescriptorAllocator::GetRecordingReadView() const noexcept
{
	return m_recordingReadView.load(std::memory_order_acquire);
}
