#include "PCH.h"

#include "Shaders/GlobalShaderMap.h"
#include "Shaders/ShaderMapRanges.h"

#include <algorithm>
#include <utility>

using ShaderMapRanges::ContainsRange;
using ShaderMapRanges::ContainsString;

ShaderFeatureFlags operator|(ShaderFeatureFlags lhs, ShaderFeatureFlags rhs) noexcept
{
	return static_cast<ShaderFeatureFlags>(static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs));
}

ShaderFeatureFlags& operator|=(ShaderFeatureFlags& lhs, ShaderFeatureFlags rhs) noexcept
{
	lhs = lhs | rhs;
	return lhs;
}

bool HasShaderFeature(ShaderFeatureFlags value, ShaderFeatureFlags flag) noexcept
{
	return (static_cast<std::uint32_t>(value) & static_cast<std::uint32_t>(flag)) == static_cast<std::uint32_t>(flag);
}

const GlobalShaderMapEntry* GlobalShaderMap::Find(ShaderTypeId shaderType, ShaderTarget target) const noexcept
{
	const auto found = std::lower_bound(
	    m_entries.begin(),
	    m_entries.end(),
	    std::pair{shaderType, target},
	    [](const GlobalShaderMapEntry& entry, const std::pair<ShaderTypeId, ShaderTarget>& key)
	    { return entry.ShaderType < key.first || (entry.ShaderType == key.first && entry.Target < key.second); });
	return found != m_entries.end() && found->ShaderType == shaderType && found->Target == target ? &*found : nullptr;
}

std::string_view GlobalShaderMap::ResolveString(ShaderMapStringRef ref) const noexcept
{
	if (!ContainsString(m_stringTable, ref))
	{
		return {};
	}
	return {reinterpret_cast<const char*>(m_stringTable.data() + ref.OffsetInBytes), ref.SizeInBytes};
}

std::span<const ShaderMapBindingRecord> GlobalShaderMap::GetBindings(const GlobalShaderMapEntry& entry) const noexcept
{
	if (!ContainsRange(m_bindingRecords.size(), entry.BindingRecordOffset, entry.BindingRecordCount))
	{
		return {};
	}
	return std::span<const ShaderMapBindingRecord>(m_bindingRecords).subspan(entry.BindingRecordOffset, entry.BindingRecordCount);
}

const CookedShaderReflectionRecord& GlobalShaderMap::GetReflection(const GlobalShaderMapEntry& entry) const noexcept
{
	return m_reflectionRecords[entry.ReflectionRecordIndex];
}
