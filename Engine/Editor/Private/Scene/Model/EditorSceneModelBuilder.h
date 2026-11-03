#pragma once

#include "Scene/Model/EditorSceneModel.h"

#include <cstdint>
#include <utility>
#include <functional>
#include <memory>

struct EditorSceneSource final
{
	std::function<WorldReadView()> AcquireReadView;
	std::function<std::uint64_t()> WorldGeneration;
	std::function<WorldMaterialVariantView()> MaterialVariants;
};

class EditorSceneModelBuilder final
{
public:
	explicit EditorSceneModelBuilder(EditorSceneSource source) :
	    m_source(std::move(source))
	{
	}

	std::shared_ptr<const EditorSceneModel> Update();

private:
	EditorSceneSource m_source;
	std::shared_ptr<const EditorSceneModel> m_current;
};
