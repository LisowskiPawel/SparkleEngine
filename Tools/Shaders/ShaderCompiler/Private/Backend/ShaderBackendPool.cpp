#include "PCH.h"

#include "Backend/ShaderBackendPool.h"

#include "Backend/IShaderBackend.h"
#include "Backend/ShaderBackendFactory.h"

IShaderBackend& ShaderBackendPool::ResolveAndAcquire(std::string_view sourcePath, ShaderTarget target, std::string_view requestedName)
{
	const std::string backendName = ResolveShaderBackendName(sourcePath, target, requestedName);
	const auto existing = m_backends.find(backendName);
	if (existing != m_backends.end())
	{
		return *existing->second;
	}

	std::unique_ptr<IShaderBackend> backend = CreateShaderBackend(backendName);

	IShaderBackend& backendReference = *backend;
	m_backends.emplace(backendName, std::move(backend));
	return backendReference;
}
