#include "PCH.h"

#include "World/Resources/MaterialVariantResourceStore.h"

#include <utility>

void MaterialVariantResourceStore::Append(std::vector<MaterialVariantDesc> variants, std::vector<MaterialVariantBinding> bindings)
{
	const MaterialVariantIndex base = static_cast<MaterialVariantIndex>(m_variants.size());
	for (MaterialVariantDesc& variant : variants)
	{
		m_variants.push_back(std::move(variant));
	}
	for (MaterialVariantBinding& binding : bindings)
	{
		if (binding.Variant == kInvalidMaterialVariantIndex || !binding.Entity.IsValid())
		{
			continue;
		}
		binding.Variant += base;
		m_bindings.push_back(binding);
	}
}

WorldMaterialVariantView MaterialVariantResourceStore::CaptureView() const
{
	WorldMaterialVariantView view;
	view.Active = m_active;
	view.Names.reserve(m_variants.size());
	for (const MaterialVariantDesc& variant : m_variants)
	{
		view.Names.push_back(variant.name);
	}
	return view;
}
