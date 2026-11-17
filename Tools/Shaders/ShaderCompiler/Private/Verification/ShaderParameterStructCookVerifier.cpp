#include "PCH.h"

#include "Verification/ShaderParameterStructCookVerifier.h"

#include "Cooking/ShaderCookDiagnostics.h"
#include "Core/Public/Diagnostics/Error.h"
#include "ShaderDebugArtifactSet.h"
#include "Verification/ShaderParameterStructVerifier.h"

#include <format>
#include <utility>

void VerifyCookedShaderParameters(const ShaderCompileJob& job, const CookedStageBuild& compiledStage, ShaderDebugArtifactSet* debugArtifacts)
{
	if (!job.Request.ParameterStruct.has_value())
	{
		return;
	}

	ShaderParameterStructVerificationResult result = VerifyShaderParameterStruct(*job.Request.ParameterStruct, compiledStage.reflection, job.Request.UnitKind == ShaderCompileUnitKind::Library);

	if (!result.mismatches.empty())
	{
		throw Diagnostics::Error(
		    std::format(
		        "SC2001 {} parameter-struct '{}' verification failed: {}",
		        FormatShaderCompileJob(job, compiledStage.backendName, job.Request.Target),
		        job.Request.ParameterStruct->Name,
		        result.mismatches.front()));
	}
	if (debugArtifacts != nullptr)
	{
		debugArtifacts->ParameterVerification = std::move(result);
	}
}
