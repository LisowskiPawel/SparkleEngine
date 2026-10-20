#include "PCH.h"

#include "Diagnostics/GpuCrash/GpuCrashInputVerification.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Json/JsonReader.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "Core/Public/Paths/PathUtils.h"
#include "RHI/Public/Shaders/GlobalShaderMap.h"

#include <span>
#include <tuple>
#include <vector>

namespace GpuCrashInputVerification
{
	using ExportedShaderJobs = std::set<std::tuple<std::uint64_t, std::string, std::uint64_t, std::string>>;

	static void VerifyShaderDebugData(
	    std::string_view format,
	    const std::filesystem::path& requestPath,
	    std::string_view request,
	    const std::filesystem::path& bundleRoot,
	    const std::set<std::wstring>& files)
	{
		const auto information = ReadRequiredString(request, "debugInformation");
		if (format == "Dxil")
		{
			const auto symbolPath = ResolveReceiptFile(requestPath.parent_path(), ReadRequiredString(request, "symbolName"));
			if (information != "full-pdb" || !files.contains(Paths::MakePathKey(symbolPath)))
			{
				throw Diagnostics::Error("Shader export is missing its full PDB receipt.");
			}
		}
		else if (information != "embedded-spirv")
		{
			throw Diagnostics::Error("Shader export does not request full SPIR-V debug information.");
		}
		std::vector<std::string_view> sources;
		if (!Json::TryReadArrayProperty(request, "sources", sources) || sources.empty())
		{
			throw Diagnostics::Error("Shader export omits transitive source identity.");
		}
		for (const auto source : sources)
		{
			const auto sourceJson = "{\"path\":" + std::string(source) + "}";
			const auto sourcePath = ResolveReceiptFile(bundleRoot, ReadRequiredString(sourceJson, "path"));
			if (!files.contains(Paths::MakePathKey(sourcePath)))
			{
				throw Diagnostics::Error("Shader source is outside verified export membership.");
			}
		}
	}

	static ExportedShaderJobs ReadExportedShaderJobs(ERhiBackendApi api, const std::filesystem::path& bundleRoot, std::string_view bundle, const std::set<std::wstring>& files)
	{
		std::vector<std::string_view> jobs;
		if (!Json::TryReadArrayProperty(bundle, "jobs", jobs) || jobs.empty() || jobs.size() > files.size())
		{
			throw Diagnostics::Error("Shader job membership is missing or exceeds verified export files.");
		}
		ExportedShaderJobs exportedJobs;
		for (const auto job : jobs)
		{
			// Jobs name the receipt-owned compile requests.
			std::string requestName;
			const std::string object = "{\"job\":" + std::string(job) + "}";
			if (!Json::TryReadStringProperty(object, "job", requestName))
			{
				throw Diagnostics::Error("Shader job membership is malformed.");
			}
			const auto requestPath = ResolveReceiptFile(bundleRoot, requestName);
			if (!files.contains(Paths::MakePathKey(requestPath)))
			{
				throw Diagnostics::Error("Shader job is outside verified export membership.");
			}
			const auto request = ReadReceipt(requestPath);
			const auto format = ReadRequiredString(request, "format");
			if ((api == ERhiBackendApi::D3D12 && format == "Dxil") || (api == ERhiBackendApi::Vulkan && format == "SpirV"))
			{
				const auto codePath = requestPath.parent_path() / (format == "Dxil" ? "shader.dxil" : "shader.spv");
				if (!files.contains(Paths::MakePathKey(codePath)) || HashFile(codePath) != ReadRequiredString(request, "codeSha256"))
				{
					throw Diagnostics::Error("Shader request identity differs from its exported code.");
				}
				VerifyShaderDebugData(format, requestPath, request, bundleRoot, files);
				std::uint64_t type = 0;
				std::uint64_t input = 0;
				if (!Formatting::TryParseHexUInt64(ReadRequiredString(request, "shaderTypeId"), type) || !Formatting::TryParseHexUInt64(ReadRequiredString(request, "compileInputHash"), input)
				    || !exportedJobs.emplace(type, ReadRequiredString(request, "target"), input, ReadRequiredString(request, "codeSha256")).second)
				{
					throw Diagnostics::Error("Shader export has invalid or duplicate permutation identity.");
				}
			}
		}
		return exportedJobs;
	}

	static void VerifyShaderJobs(ERhiBackendApi api, const std::filesystem::path& bundleRoot, std::string_view bundle, const std::set<std::wstring>& files)
	{
		const auto library = CookedShaderLibrary::Open(Filesystem::GetCookedShaderLibraryPath());
		const auto map = GlobalShaderMap::Open(Filesystem::GetGlobalShaderMapPath(), library);
		const auto exportedJobs = ReadExportedShaderJobs(api, bundleRoot, bundle, files);
		std::size_t matched = 0;
		for (const auto& entry : map.GetEntries())
		{
			if (IsSpirVTarget(entry.Target) != (api == ERhiBackendApi::Vulkan))
			{
				continue;
			}
			const auto* code = library.Find(entry.CodeHash);
			if (code == nullptr)
			{
				throw Diagnostics::Error("Active shader code is missing.");
			}
			const auto bytes = library.GetBytecode(*code);
			std::string hash;
			std::string error;
			if (!Hash::TrySha256Hex(std::span(static_cast<const std::byte*>(bytes.Data), bytes.Size), hash, error)
			    || !exportedJobs.contains({entry.ShaderType, GetShaderTargetName(entry.Target), entry.CompileInputHash, hash}))
			{
				throw Diagnostics::Error("Export omits an active shader or permutation; perform a complete diagnostic shader cook.");
			}
			++matched;
		}
		if (matched == 0)
		{
			throw Diagnostics::Error("Shader export has no code for the selected API.");
		}
	}

	std::string VerifyShaderBundle(const std::filesystem::path& exportPath, std::string_view product, ERhiBackendApi api)
	{
		const auto bundle = ReadReceipt(exportPath);
		if (ReadRequiredString(bundle, "scope") != "registered-catalog")
		{
			throw Diagnostics::Error("Aftermath requires a complete registered-catalog export, not a selective shader cook.");
		}
		const auto bundleRoot = exportPath.parent_path();
		const auto files = VerifyReceiptFiles(bundleRoot, bundle, "files");
		const auto compilerPath = bundleRoot / "product-receipt.json";
		if (!files.contains(Paths::MakePathKey(compilerPath)))
		{
			throw Diagnostics::Error("Shader export has no build-bound compiler receipt.");
		}
		const auto compiler = ReadReceipt(compilerPath);
		for (const auto key : {"revision", "sourceInputsSha256", "dirtyPatchSha256"})
		{
			if (ReadRequiredString(compiler, key) != ReadRequiredString(product, key))
			{
				throw Diagnostics::Error("Shader export and product belong to different source candidates.");
			}
		}
		if (ReadRequiredString(compiler, "product") != "ShaderCompiler" || ReadRequiredString(compiler, "variant") != "gpu-crash"
		    || ReadRequiredString(compiler, "executableSha256") != ReadRequiredString(bundle, "shaderCompilerExecutableSha256")
		    || ReadRequiredString(bundle, "globalShaderMapSha256") != HashFile(Filesystem::GetGlobalShaderMapPath())
		    || ReadRequiredString(bundle, "cookedShaderLibrarySha256") != HashFile(Filesystem::GetCookedShaderLibraryPath()))
		{
			throw Diagnostics::Error("Shader export does not match the active cooked publication.");
		}
		VerifyShaderJobs(api, bundleRoot, bundle, files);
		return bundle;
	}
}
