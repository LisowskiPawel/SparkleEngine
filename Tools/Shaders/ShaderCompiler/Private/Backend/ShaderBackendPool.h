#pragma once

#include "Backend/IShaderBackend.h"
#include "RHI/Public/Shaders/ShaderTarget.h"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

class ShaderBackendPool final
{
public:
	IShaderBackend& ResolveAndAcquire(std::string_view sourcePath, ShaderTarget target, std::string_view requestedName);

private:
	IShaderBackend& Acquire(std::string_view backendName, ShaderTarget target);

	std::unordered_map<std::string, std::unique_ptr<IShaderBackend>> m_backends;
};
