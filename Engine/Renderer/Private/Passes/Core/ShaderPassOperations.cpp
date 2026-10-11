#include "PCH.h"
#include "Passes/Core/ShaderPassOperations.h"

namespace ShaderPassOperations
{
	std::vector<const char*> BuildCompiledBindingNames(const RenderBindingLayout& bindingLayout) noexcept
	{
		std::vector<const char*> bindingNames;
		bindingNames.reserve(bindingLayout.GetBindingCount());
		for (std::size_t index = 0; index < bindingLayout.GetBindingCount(); ++index)
		{
			bindingNames.push_back(bindingLayout.GetBindings()[index].Name);
		}

		return bindingNames;
	}
}
