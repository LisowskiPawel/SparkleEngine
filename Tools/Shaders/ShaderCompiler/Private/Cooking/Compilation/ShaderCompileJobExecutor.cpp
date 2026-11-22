#include "PCH.h"

#include "Cooking/Compilation/ShaderCompileJobExecutor.h"

#include "Backend/IShaderBackend.h"
#include "Backend/ShaderBackendFactory.h"
#include "Cooking/ShaderCookDiagnostics.h"
#include "Cooking/Compilation/ShaderCompileJob.h"
#include "Cooking/CookedStageBuild.h"
#include "Compiler/ShaderCompileRequest.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Compiler/Output/CompiledShader.h"

#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace ShaderCompileJobExecutor
{
	ShaderCompileResult Execute(const ShaderCompileJob& job)
	{
		try
		{
			std::unique_ptr<IShaderBackend> backend = CreateShaderBackend(job.BackendName);
			const bool bNameMatches = backend->GetBackendName() == job.BackendName;
			const bool bVersionMatches = backend->GetBackendVersion() == job.BackendVersion;
			if (!bNameMatches || !bVersionMatches)
			{
				throw Diagnostics::Error("Shader compiler identity changed after planning.");
			}
			CompiledShader compiledShader = backend->Compile(job.Request);
			std::vector<std::uint8_t> bytecode = compiledShader.TakeBytecode();
			const std::uint64_t bytecodeHash = Hash::Fnv1a64(bytecode.data(), bytecode.size());
			CookedStageBuild output{
			    .stage = job.Request.Stage,
			    .format = IsSpirVTarget(job.Request.Target) ? ShaderBinaryFormat::SpirV : ShaderBinaryFormat::Dxil,
			    .sourcePath = job.Request.VirtualSourcePath,
			    .entryPoint = job.Request.EntryPoint,
			    .backendName = job.BackendName,
			    .codegenTarget = GetShaderTargetName(job.Request.Target),
			    .backendVersion = job.BackendVersion,
			    .sourceHash = job.SourceContentHash,
			    .includeClosureHash = job.DependencyClosureHash,
			    .requestHash = job.RequestHash,
			    .compileInputHash = job.InputHash,
			    .bytecode = std::move(bytecode),
			    .bytecodeHash = bytecodeHash,
			    .reflection = compiledShader.TakeReflection()};

			return ShaderCompileResult{
			    .ShaderType = job.Request.ShaderType,
			    .Target = job.Request.Target,
			    .InputHash = job.InputHash,
			    .Output = std::move(output),
			    .DebugArtifacts = compiledShader.TakeDebugArtifacts()};
		}
		catch (const Diagnostics::Error& error)
		{
			const std::string jobContext = FormatShaderCompileJob(job);
			throw Diagnostics::Error(std::format("Failed to compile {} - {}", jobContext, error.what()));
		}
	}
}
