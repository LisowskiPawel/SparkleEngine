#include "PCH.h"

#include "Cli/ListTargetsCommand.h"

#include "RHI/Public/Shaders/ShaderTarget.h"
#include "Constants/ShaderCompilerConstants.h"

#include <iostream>

int ListShaderTargets()
{
	for (std::uint16_t candidate = static_cast<std::uint16_t>(ShaderTarget::DxilSm60); candidate <= static_cast<std::uint16_t>(ShaderTarget::SpirV16); ++candidate)
	{
		std::cout << GetShaderTargetName(static_cast<ShaderTarget>(candidate)) << '\n';
	}
	return kExitCodeSuccess;
}
