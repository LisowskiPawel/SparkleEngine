#include "PCH.h"

#include "Verification/ShaderParameterStructCookVerifier.h"

#include "Cooking/ShaderCookDiagnostics.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Json/JsonWriter.h"
#include "ShaderDebugArtifactSet.h"
#include "Verification/ShaderParameterStructVerifier.h"

#include <format>

static void WriteSkippedReport(ShaderDebugArtifactSet* debugArtifacts, std::string_view reason);

void VerifyCookedShaderParameters(const ShaderCompileJob& job, const CookedStageBuild& compiledStage, ShaderDebugArtifactSet* debugArtifacts)
{
	if (!job.Request.ParameterStruct.has_value())
	{
		WriteSkippedReport(debugArtifacts, "no parameter-struct descriptor declared for this shader stage");
		return;
	}

	const ShaderParameterStructVerificationResult verificationResult = VerifyShaderParameterStruct(
	    *job.Request.ParameterStruct,
	    compiledStage.reflection,
	    job.Request.UnitKind == ShaderCompileUnitKind::Library);

	if (debugArtifacts != nullptr)
	{
		debugArtifacts->ParameterMatchReportJson = verificationResult.BuildJsonReport();
	}

	if (!verificationResult.mismatches.empty())
	{
		throw Diagnostics::Error(
		    std::format(
		        "SC2001 {} parameter-struct '{}' verification failed: {}",
		        FormatShaderCompileJob(job, compiledStage.backendName, job.Request.Target),
		        job.Request.ParameterStruct->Name,
		        verificationResult.mismatches.front()));
	}
}

static void WriteSkippedReport(ShaderDebugArtifactSet* debugArtifacts, std::string_view reason)
{
	if (debugArtifacts == nullptr)
	{
		return;
	}

	Json::ObjectWriter writer;
	writer.WriteString("status", "skipped");
	writer.WriteString("reason", reason);
	debugArtifacts->ParameterMatchReportJson = writer.Finish();
}
