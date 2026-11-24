#pragma once

#include "PassParameterSet.h"
#include "../FrameGraph/FrameGraphAccelerationStructureHandle.h"
#include "../FrameGraph/FrameGraphAttachment.h"
#include "Renderer/Public/RendererAPI.h"

#include <cstdint>
#include <array>
#include <cstddef>
#include <type_traits>

namespace ShaderParameterFields
{
	template <typename THandle, std::size_t ArrayCount> class ResourceArrayField
	{
	public:
		static_assert(ArrayCount > 0, "Shader parameter field arrays must contain at least one element.");

		using HandleType = THandle;
		static constexpr std::uint32_t FieldArrayCount = static_cast<std::uint32_t>(ArrayCount);
		static constexpr bool bUsesGraphResource = true;

		ResourceArrayField() = default;

		template <std::size_t CountValue = ArrayCount, typename = std::enable_if_t<CountValue == 1>> ResourceArrayField& operator=(THandle handle) noexcept
		{
			m_values[0] = handle;
			return *this;
		}

		void Set(std::size_t index, THandle handle) noexcept { m_values[index] = handle; }

		const THandle& operator[](std::size_t index) const noexcept { return m_values[index]; }

		THandle& operator[](std::size_t index) noexcept { return m_values[index]; }

		const std::array<THandle, ArrayCount>& GetValues() const noexcept { return m_values; }

	private:
		std::array<THandle, ArrayCount> m_values = {};
	};
}

template <typename TValue = void, std::size_t ArrayCount = 1> class ShaderTexture2D final : public ShaderParameterFields::ResourceArrayField<FrameGraphTextureHandle, ArrayCount>
{
public:
	using Base = ShaderParameterFields::ResourceArrayField<FrameGraphTextureHandle, ArrayCount>;
	using Semantic = ReadTexture;
	using ValueType = TValue;
	using Base::operator=;
	using Base::Base;
};

template <typename TValue = void, std::size_t ArrayCount = 1> class ShaderTexture3D final : public ShaderParameterFields::ResourceArrayField<FrameGraphTextureHandle, ArrayCount>
{
public:
	using Base = ShaderParameterFields::ResourceArrayField<FrameGraphTextureHandle, ArrayCount>;
	using Semantic = ReadTexture;
	using ValueType = TValue;
	using Base::operator=;
	using Base::Base;
};

template <typename TValue = void, std::size_t ArrayCount = 1> class ShaderTextureCube final : public ShaderParameterFields::ResourceArrayField<FrameGraphTextureHandle, ArrayCount>
{
public:
	using Base = ShaderParameterFields::ResourceArrayField<FrameGraphTextureHandle, ArrayCount>;
	using Semantic = ReadTexture;
	using ValueType = TValue;
	using Base::operator=;
	using Base::Base;
};

class SPARKLE_RENDERER_API ShaderTexture2DSRV final
{
public:
	using Semantic = ReadTexture;
	static constexpr std::uint32_t FieldArrayCount = 1;
	static constexpr bool bUsesGraphResource = false;

	ShaderTexture2DSRV();

	ShaderTexture2DSRV& operator=(RhiDescriptorTableBinding descriptorTable) noexcept
	{
		m_descriptorTable = descriptorTable;
		return *this;
	}

	RhiDescriptorTableBinding GetDescriptorTable() const noexcept { return m_descriptorTable; }

	void Reset() noexcept { m_descriptorTable = {}; }

private:
	RhiDescriptorTableBinding m_descriptorTable = {};
};

template <std::size_t ArrayCount> class ShaderTexture2DTableSRV final
{
public:
	static_assert(ArrayCount > 0, "Shader texture descriptor tables must contain at least one element.");

	using Semantic = ReadTexture;
	static constexpr std::uint32_t FieldArrayCount = static_cast<std::uint32_t>(ArrayCount);
	static constexpr bool bUsesGraphResource = false;

	ShaderTexture2DTableSRV() = default;

	ShaderTexture2DTableSRV& operator=(RhiDescriptorTableBinding descriptorTable) noexcept
	{
		m_descriptorTable = descriptorTable;
		return *this;
	}

	RhiDescriptorTableBinding GetDescriptorTable() const noexcept { return m_descriptorTable; }

	void Reset() noexcept { m_descriptorTable = {}; }

private:
	RhiDescriptorTableBinding m_descriptorTable = {};
};

class SPARKLE_RENDERER_API ShaderAccelerationStructure final
{
public:
	using Semantic = AccelerationStructure;
	static constexpr std::uint32_t FieldArrayCount = 1;
	static constexpr bool bUsesGraphResource = true;

	ShaderAccelerationStructure();

	ShaderAccelerationStructure& operator=(FrameGraphAccelerationStructureHandle handle) noexcept
	{
		m_handle = handle;
		return *this;
	}

	FrameGraphAccelerationStructureHandle GetHandle() const noexcept { return m_handle; }

	void Reset() noexcept { m_handle = FrameGraphAccelerationStructureHandle::Invalid(); }

private:
	FrameGraphAccelerationStructureHandle m_handle = FrameGraphAccelerationStructureHandle::Invalid();
};

template <typename TValue = void, std::size_t ArrayCount = 1> class ShaderRWTexture2D final : public ShaderParameterFields::ResourceArrayField<FrameGraphTextureHandle, ArrayCount>
{
public:
	using Base = ShaderParameterFields::ResourceArrayField<FrameGraphTextureHandle, ArrayCount>;
	using Semantic = RWTexture;
	using ValueType = TValue;
	using Base::operator=;
	using Base::Base;
};

class ShaderRenderTarget final
{
public:
	using Semantic = RenderTarget;
	static constexpr std::uint32_t FieldArrayCount = 1;
	static constexpr bool bUsesGraphResource = true;

	ShaderRenderTarget& operator=(FrameGraphAttachmentBinding binding) noexcept
	{
		m_binding = binding;
		return *this;
	}

	const FrameGraphTextureHandle& operator[](std::size_t) const noexcept { return m_binding.Handle; }

	const FrameGraphAttachmentBinding& GetBinding() const noexcept { return m_binding; }

private:
	FrameGraphAttachmentBinding m_binding = {};
};

class ShaderDepthTarget final
{
public:
	using Semantic = DepthTarget;
	static constexpr std::uint32_t FieldArrayCount = 1;
	static constexpr bool bUsesGraphResource = true;

	ShaderDepthTarget& operator=(FrameGraphAttachmentBinding binding) noexcept
	{
		m_binding = binding;
		return *this;
	}

	const FrameGraphTextureHandle& operator[](std::size_t) const noexcept { return m_binding.Handle; }

	const FrameGraphAttachmentBinding& GetBinding() const noexcept { return m_binding; }

private:
	FrameGraphAttachmentBinding m_binding = {};
};

template <typename TValue = void, std::size_t ArrayCount = 1> class ShaderBuffer final : public ShaderParameterFields::ResourceArrayField<FrameGraphBufferHandle, ArrayCount>
{
public:
	using Base = ShaderParameterFields::ResourceArrayField<FrameGraphBufferHandle, ArrayCount>;
	using Semantic = ReadBuffer;
	using ValueType = TValue;
	using Base::operator=;
	using Base::Base;
};

template <typename TValue = void, std::size_t ArrayCount = 1> class ShaderRWBuffer final : public ShaderParameterFields::ResourceArrayField<FrameGraphBufferHandle, ArrayCount>
{
public:
	using Base = ShaderParameterFields::ResourceArrayField<FrameGraphBufferHandle, ArrayCount>;
	using Semantic = RWBuffer;
	using ValueType = TValue;
	using Base::operator=;
	using Base::Base;
};

template <typename TValue> class ShaderUniform final
{
public:
	using Semantic = UniformData<TValue>;
	using ValueType = TValue;
	static constexpr std::uint32_t FieldArrayCount = 1;
	static constexpr bool bUsesGraphResource = false;

	ShaderUniform() = default;

	ShaderUniform& operator=(const TValue& value) noexcept
	{
		m_value = value;
		m_bIsBound = true;
		return *this;
	}

	const TValue* GetValue() const noexcept { return m_bIsBound ? &m_value : nullptr; }

	bool IsBound() const noexcept { return m_bIsBound; }

	void Reset() noexcept
	{
		m_value = {};
		m_bIsBound = false;
	}

private:
	TValue m_value = {};
	bool m_bIsBound = false;
};

class SPARKLE_RENDERER_API ShaderSamplerSet final
{
public:
	using Semantic = SamplerSet;
	static constexpr std::uint32_t FieldArrayCount = 1;
	static constexpr bool bUsesGraphResource = false;

	ShaderSamplerSet();

	ShaderSamplerSet& operator=(RhiSamplerDesc sampler) noexcept
	{
		m_sampler = sampler;
		m_bIsBound = true;
		return *this;
	}

	const RhiSamplerDesc& GetSampler() const noexcept { return m_sampler; }

	bool IsBound() const noexcept { return m_bIsBound; }

	void Reset() noexcept
	{
		m_sampler = {};
		m_bIsBound = false;
	}

private:
	RhiSamplerDesc m_sampler = {};

	bool m_bIsBound = false;
};

template <std::size_t ArrayCount> bool BindParameterField(PassParameterSet& parameterSet, const char* name, const ShaderParameterFields::ResourceArrayField<FrameGraphTextureHandle, ArrayCount>& field)
{
	return parameterSet.SetTextureArray(name, field.GetValues());
}

template <std::size_t ArrayCount> bool BindParameterField(PassParameterSet& parameterSet, const char* name, const ShaderParameterFields::ResourceArrayField<FrameGraphBufferHandle, ArrayCount>& field)
{
	return parameterSet.SetBufferArray(name, field.GetValues());
}

SPARKLE_RENDERER_API bool BindParameterField(PassParameterSet& parameterSet, const char* name, const ShaderTexture2DSRV& field);

template <std::size_t ArrayCount> inline bool BindParameterField(PassParameterSet& parameterSet, const char* name, const ShaderTexture2DTableSRV<ArrayCount>& field)
{
	return parameterSet.SetShaderResourceView(name, field.GetDescriptorTable());
}

SPARKLE_RENDERER_API bool BindParameterField(PassParameterSet& parameterSet, const char* name, const ShaderAccelerationStructure& field);

SPARKLE_RENDERER_API bool BindParameterField(PassParameterSet& parameterSet, const char* name, const ShaderRenderTarget& field);
SPARKLE_RENDERER_API bool BindParameterField(PassParameterSet& parameterSet, const char* name, const ShaderDepthTarget& field);

template <typename TValue> bool BindParameterField(PassParameterSet& parameterSet, const char* name, const ShaderUniform<TValue>& field)
{
	if (!field.IsBound())
	{
		return false;
	}

	return parameterSet.SetUniformDataReference(name, *field.GetValue());
}

SPARKLE_RENDERER_API bool BindParameterField(PassParameterSet& parameterSet, const char* name, const ShaderSamplerSet& field);
