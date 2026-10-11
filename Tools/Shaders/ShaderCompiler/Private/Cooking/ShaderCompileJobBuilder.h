#pragma once

#include "Cooking/ShaderCompileJob.h"
#include "Cooking/ShaderCookTypes.h"
#include "ShaderContractCatalog.h"

#include <cstdint>
#include <string_view>
#include <vector>

class ShaderBackendPool;
struct ShaderCookSettings;

class ShaderCompileJobBuilder final
{
public:
	explicit ShaderCompileJobBuilder(const ShaderContractCatalog& catalog);

	ShaderCompileJob Build(const ShaderCookSettings& settings, const ShaderCookDesc& shader, ShaderTarget target, ShaderBackendPool& backendPool) const;

private:
	ShaderCompileRequest BuildRequest(const ShaderCookSettings& settings, const ShaderCookDesc& shader, ShaderTarget target) const;
	void AppendDescriptorBindingRemaps(const ShaderCookDesc& shader, ShaderCompileRequest& request) const;
	static ShaderCompileFeatureFlags BuildRequiredFeatures(ShaderFeatureFlags features) noexcept;
	static const ShaderSourceMountTable& GetSourceMounts();
	static ShaderCompileInputHash BuildInputHash(
	    std::uint64_t sourceContentHash,
	    std::uint64_t dependencyClosureHash,
	    std::uint64_t requestHash,
	    std::string_view backendName,
	    std::uint64_t backendVersion);

	// The validated catalog outlives this operation-local binding assignment.
	std::vector<const PassParameterDesc*> m_descriptorBindings;
};
