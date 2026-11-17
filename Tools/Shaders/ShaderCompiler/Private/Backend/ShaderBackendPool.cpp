#include "PCH.h"

#include "Backend/ShaderBackendPool.h"

#include "Backend/IShaderBackend.h"
#include "Backend/ShaderBackendFactory.h"

#include "Core/Public/Diagnostics/Error.h"

IShaderBackend& ShaderBackendPool::ResolveAndAcquire(std::string_view sourcePath, ShaderTarget target, std::string_view requestedName)
{
	const std::string backendName = ResolveShaderBackendName(sourcePath, target, requestedName);
	return Acquire(backendName, target);
}

IShaderBackend& ShaderBackendPool::Acquire(std::string_view backendName, ShaderTarget target)
{
	const auto existing = m_backends.find(std::string(backendName));
	if (existing != m_backends.end())
	{
		return *existing->second;
	}

	std::unique_ptr<IShaderBackend> backend = CreateShaderBackend(backendName);

	const std::string resolvedName(backend->GetBackendName());
	if (!backend->GetCapabilities().SupportsTarget(target))
	{
		throw Diagnostics::Error(std::string{"Shader backend '"} + resolvedName + "' does not support target '" + GetShaderTargetName(target) + "'.");
	}

	IShaderBackend& backendReference = *backend;
	m_backends.emplace(resolvedName, std::move(backend));
	return backendReference;
}
