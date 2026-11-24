#pragma once

#include "RHI/Public/Resources/RhiResourceHandles.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

class RhiResourceService;

class OwnedStructuredBuffer final
{
public:
	OwnedStructuredBuffer() noexcept = default;

	~OwnedStructuredBuffer() noexcept;

	OwnedStructuredBuffer(const OwnedStructuredBuffer&) = delete;
	OwnedStructuredBuffer& operator=(const OwnedStructuredBuffer&) = delete;
	OwnedStructuredBuffer(OwnedStructuredBuffer&& other) noexcept;
	OwnedStructuredBuffer& operator=(OwnedStructuredBuffer&& other) noexcept;

	static OwnedStructuredBuffer Upload(RhiResourceService& resourceService, const void* data, std::size_t sizeInBytes, std::uint32_t strideInBytes, std::wstring_view debugName);

	template <typename TValue> static OwnedStructuredBuffer Upload(RhiResourceService& resourceService, std::span<const TValue> values, std::wstring_view debugName)
	{
		return Upload(resourceService, values.data(), values.size_bytes(), static_cast<std::uint32_t>(sizeof(TValue)), debugName);
	}

	bool IsValid() const noexcept { return static_cast<bool>(m_resource); }

	explicit operator bool() const noexcept { return IsValid(); }

	RhiOwnedResourceHandle GetResource() const noexcept { return m_resource; }

	bool Write(std::size_t destinationOffsetInBytes, const void* data, std::size_t sizeInBytes) noexcept;
	void Reset() noexcept;

private:
	RhiResourceService* m_resourceService = nullptr;

	RhiOwnedResourceHandle m_resource = {};
};
