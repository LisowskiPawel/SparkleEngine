#include "PCH.h"

#include "Cooking/ShaderCookDiagnostics.h"

#include "Compiler/ShaderCompileRequest.h"
#include "Cooking/Compilation/ShaderCompileJob.h"

#include <format>

std::string FormatShaderCompileRequest(const ShaderCompileRequest& request, std::string_view backendName)
{
	return std::format(
	    "shader type '{}' source '{}' entry '{}' stage '{}' backend '{}' target '{}'",
	    request.ShaderTypeName,
	    request.VirtualSourcePath,
	    request.EntryPoint,
	    GetShaderStagePrefix(request.Stage),
	    backendName,
	    GetShaderTargetName(request.Target));
}

std::string FormatShaderCompileJob(const ShaderCompileJob& job)
{
	const std::string requestDescription = FormatShaderCompileRequest(job.Request, job.BackendName);
	return std::format("{} profile '{}' compileInputHash {:016X}", requestDescription, job.TargetProfile, job.InputHash);
}
