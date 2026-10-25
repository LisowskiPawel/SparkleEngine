#include "PCH.h"

#include "Cooking/ShaderCompileJobExecutor.h"

#include "Backend/IShaderBackend.h"
#include "Backend/ShaderBackendFactory.h"
#include "Cooking/ShaderCookDiagnostics.h"
#include "Cooking/ShaderCompileJob.h"
#include "Cooking/CookedStageBuild.h"
#include "Compiler/ShaderCompileRequest.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Hash/HashUtils.h"
#include "ShaderDebugArtifactSet.h"

#include <format>
#include <memory>
#include <string>
#include <utility>

namespace ShaderCompileJobExecutor
{
	static CookedStageBuild CompileStage(IShaderBackend& backend, const ShaderCompileRequest& request, ShaderDebugArtifactSet* outDebugArtifacts)
	{
		const ShaderBackendCapabilities capabilities = backend.GetCapabilities();
		if (!capabilities.SupportsTarget(request.Target))
		{
			throw Diagnostics::Error(std::string{"Active shader backend does not support target '"} + GetShaderTargetName(request.Target) + "'.");
		}
		if (HasShaderCompileFeature(request.RequiredFeatures, ShaderCompileFeatureFlags::RayTracingPipeline) && !capabilities.SupportsRayTracingLibrary(request.Target))
		{
			throw Diagnostics::Error(std::string{"Active shader backend does not support shader libraries for target '"} + GetShaderTargetName(request.Target) + "'.");
		}
		if (HasShaderCompileFeature(request.RequiredFeatures, ShaderCompileFeatureFlags::InlineRayQuery) && !capabilities.SupportsInlineRayQuery(request.Target))
		{
			throw Diagnostics::Error(std::string{"Active shader backend does not support inline ray queries for target '"} + GetShaderTargetName(request.Target) + "'.");
		}

		CompiledShader compiledShader = backend.Compile(request);
		const ShaderBytecode bytecode = compiledShader.GetBytecode();
		if (!bytecode.IsValid())
		{
			throw Diagnostics::Error("Backend returned empty bytecode for shader source '" + request.VirtualSourcePath + "'.");
		}

		CookedStageBuild compiledStage;
		compiledStage.stage = request.Stage;
		compiledStage.format = IsSpirVTarget(request.Target) ? ShaderBinaryFormat::SpirV : ShaderBinaryFormat::Dxil;
		compiledStage.sourcePath = request.VirtualSourcePath;
		compiledStage.entryPoint = request.EntryPoint;
		compiledStage.backendName.assign(backend.GetBackendName());
		compiledStage.codegenTarget.assign(GetShaderTargetName(request.Target));
		compiledStage.backendVersion = backend.GetBackendVersion();
		compiledStage.bytecode = compiledShader.TakeBytecode();
		compiledStage.bytecodeHash = Hash::Fnv1a64(compiledStage.bytecode.data(), compiledStage.bytecode.size());
		compiledStage.reflection = compiledShader.TakeReflection();

		if (outDebugArtifacts != nullptr)
		{
			*outDebugArtifacts = compiledShader.TakeDebugArtifacts();
		}

		return compiledStage;
	}

	ShaderCompileResult Execute(const ShaderCompileJob& job)
	{
		std::unique_ptr<IShaderBackend> backend = CreateShaderBackend(job.BackendName);
		ShaderDebugArtifactSet debugArtifacts;
		CookedStageBuild output;
		try
		{
			output = CompileStage(*backend, job.Request, job.Request.CaptureDebugArtifacts ? &debugArtifacts : nullptr);
		}
		catch (const Diagnostics::Error& error)
		{
			const std::string jobContext = FormatShaderCompileJob(job, backend->GetBackendName(), job.Request.Target);
			throw Diagnostics::Error(std::format("Failed to compile {} - {}", jobContext, error.what()));
		}

		output.sourceHash = job.SourceContentHash;
		output.includeClosureHash = job.DependencyClosureHash;
		output.requestHash = job.RequestHash;
		output.compileInputHash = job.InputHash;

		return ShaderCompileResult{
		    .ShaderType = job.Request.ShaderType,
		    .Target = job.Request.Target,
		    .InputHash = job.InputHash,
		    .Output = std::move(output),
		    .DebugArtifacts = std::move(debugArtifacts)};
	}
}
