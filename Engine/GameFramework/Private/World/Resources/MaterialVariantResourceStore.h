#pragma once

#include "World/WorldMaterialVariantView.h"

#include "GameFramework/Public/Scene/Materials/MaterialHandle.h"
#include "GameFramework/Public/Scene/Materials/MaterialVariant.h"
#include "GameFramework/Public/Scene/Meshes/MeshInstanceGroup.h"
#include "GameFramework/Public/World/EntityId.h"

#include <cstddef>
#include <vector>

struct MaterialVariantBinding final
{
	MaterialVariantIndex Variant = kInvalidMaterialVariantIndex;
	EntityId Entity;
	MaterialHandle Material;
};

class MaterialVariantResourceStore final
{
public:
	WorldMaterialVariantView CaptureView() const;
	void Append(std::vector<MaterialVariantDesc> variants, std::vector<MaterialVariantBinding> bindings);

	template <typename MaterialWriter> bool Apply(MaterialVariantIndex index, MaterialWriter&& writeMaterial)
	{
		if (index >= m_variants.size())
		{
			return false;
		}
		bool bApplied = false;
		for (const MaterialVariantBinding& binding : m_bindings)
		{
			if (binding.Variant == index)
			{
				bApplied = writeMaterial(binding.Entity, binding.Material) || bApplied;
			}
		}
		if (bApplied)
		{
			m_active = index;
		}
		return bApplied;
	}

	std::size_t GetCount() const noexcept { return m_variants.size(); }

private:
	std::vector<MaterialVariantDesc> m_variants;
	std::vector<MaterialVariantBinding> m_bindings;
	MaterialVariantIndex m_active = kInvalidMaterialVariantIndex;
};
