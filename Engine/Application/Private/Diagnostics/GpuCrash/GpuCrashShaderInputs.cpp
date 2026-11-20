#include "PCH.h"
#include "Core/Public/Paths/CookedAssetPaths.h"

#include "Diagnostics/GpuCrash/GpuCrashInputVerification.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Hash/HashUtils.h"
#include <nlohmann/json.hpp>
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
	    const nlohmann::json& request,
	    const std::filesystem::path& bundleRoot,
	    const std::set<std::wstring>& files)
	{
		const auto& information = request.at("debugInformation").get_ref<const std::string&>();
		if (format == "Dxil")
		{
			const auto requestDirectory = requestPath.parent_path();
			const auto& symbolName = request.at("symbolName").get_ref<const std::string&>();
			const auto symbolPath = ResolveReceiptFile(requestDirectory, symbolName);
			if (information != "full-pdb" || !files.contains(Paths::MakePathKey(symbolPath)))
			{
				throw Diagnostics::Error("Shader export is missing its full PDB receipt.");
			}
		}
		else if (information != "embedded-spirv")
		{
			throw Diagnostics::Error("Shader export does not request full SPIR-V debug information.");
		}

		const auto& sources = request.at("sources");
		if (!sources.is_array() || sources.empty())
		{
			throw Diagnostics::Error("Shader export omits transitive source identity.");
		}
		for (const auto& source : sources)
		{
			const auto sourcePath = ResolveReceiptFile(bundleRoot, source.get_ref<const std::string&>());
			if (!files.contains(Paths::MakePathKey(sourcePath)))
			{
				throw Diagnostics::Error("Shader source is outside verified export membership.");
			}
		}
	}

	static ExportedShaderJobs ReadExportedShaderJobs(ERhiBackendApi api, const std::filesystem::path& bundleRoot, const nlohmann::json& bundle, const std::set<std::wstring>& files)
	{
		const auto& jobs = bundle.at("jobs");
		const bool bInvalidJobs = !jobs.is_array() || jobs.empty();
		if (bInvalidJobs || jobs.size() > files.size())
		{
			throw Diagnostics::Error("Shader job membership is missing or exceeds verified export files.");
		}
		ExportedShaderJobs exportedJobs;
		for (const auto& job : jobs)
		{
			// Jobs name the receipt-owned compile requests.
			const auto& requestName = job.get_ref<const std::string&>();

			const auto requestPath = ResolveReceiptFile(bundleRoot, requestName);
			if (!files.contains(Paths::MakePathKey(requestPath)))
			{
				throw Diagnostics::Error("Shader job is outside verified export membership.");
			}

			const auto request = ReadReceipt(requestPath, {"format", "codeSha256", "debugInformation", "symbolName", "sources", "shaderTypeId", "compileInputHash", "target"});
			const auto& format = request.at("format").get_ref<const std::string&>();
			if (format.empty())
			{
				throw Diagnostics::Error("Shader export requires a format identity.");
			}
			const bool bDxilRequest = api == ERhiBackendApi::D3D12 && format == "Dxil";
			const bool bSpirVRequest = api == ERhiBackendApi::Vulkan && format == "SpirV";
			if (!bDxilRequest && !bSpirVRequest)
			{
				continue;
			}

			const auto codePath = requestPath.parent_path() / (bDxilRequest ? "shader.dxil" : "shader.spv");
			const auto codeKey = Paths::MakePathKey(codePath);
			if (!files.contains(codeKey))
			{
				throw Diagnostics::Error("Shader request identity differs from its exported code.");
			}

			const std::string codeHash = HashFile(codePath);
			const auto& expectedCodeHash = request.at("codeSha256").get_ref<const std::string&>();
			if (codeHash != expectedCodeHash)
			{
				throw Diagnostics::Error("Shader request identity differs from its exported code.");
			}
			VerifyShaderDebugData(format, requestPath, request, bundleRoot, files);

			std::uint64_t type = 0;
			std::uint64_t input = 0;
			const auto& shaderType = request.at("shaderTypeId").get_ref<const std::string&>();
			const bool bShaderTypeRead = Formatting::TryParseHexUInt64(shaderType, type);
			if (!bShaderTypeRead)
			{
				throw Diagnostics::Error("Shader export has invalid or duplicate permutation identity.");
			}

			const auto& compileInputHash = request.at("compileInputHash").get_ref<const std::string&>();
			const bool bCompileInputRead = Formatting::TryParseHexUInt64(compileInputHash, input);
			const auto& target = request.at("target").get_ref<const std::string&>();
			const bool bTargetKnown = bCompileInputRead && !target.empty();
			const bool bJobUnique = bTargetKnown && exportedJobs.emplace(type, target, input, expectedCodeHash).second;

			if (!bJobUnique)
			{
				throw Diagnostics::Error("Shader export has invalid or duplicate permutation identity.");
			}
		}
		return exportedJobs;
	}

	static void VerifyShaderJobs(ERhiBackendApi api, const std::filesystem::path& bundleRoot, const nlohmann::json& bundle, const std::set<std::wstring>& files)
	{
		const auto& libraryPath = Filesystem::GetCookedShaderLibraryPath();
		const auto library = CookedShaderLibrary::Open(libraryPath);
		const auto& mapPath = Filesystem::GetGlobalShaderMapPath();
		const auto map = GlobalShaderMap::Open(mapPath, library);
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
			const std::span bytecode(static_cast<const std::byte*>(bytes.Data), bytes.Size);
			std::string hash;
			std::string error;
			const bool bCodeHashed = Hash::TrySha256Hex(bytecode, hash, error);
			if (!bCodeHashed)
			{
				throw Diagnostics::Error("Export omits an active shader or permutation; perform a complete diagnostic shader cook.");
			}

			const ExportedShaderJobs::key_type job{entry.ShaderType, GetShaderTargetName(entry.Target), entry.CompileInputHash, hash};
			if (!exportedJobs.contains(job))
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

	nlohmann::json VerifyShaderBundle(const std::filesystem::path& exportPath, const nlohmann::json& product, ERhiBackendApi api)
	{
		const auto bundle = ReadReceipt(exportPath, {"scope", "files", "jobs", "shaderCompilerExecutableSha256", "globalShaderMapSha256", "cookedShaderLibrarySha256"});
		if (bundle.at("scope").get_ref<const std::string&>() != "registered-catalog")
		{
			throw Diagnostics::Error("Aftermath requires a complete registered-catalog export, not a selective shader cook.");
		}

		const auto bundleRoot = exportPath.parent_path();
		const auto files = VerifyReceiptFiles(bundleRoot, bundle.at("files"), true);
		const auto compilerPath = bundleRoot / "product-receipt.json";
		if (!files.contains(Paths::MakePathKey(compilerPath)))
		{
			throw Diagnostics::Error("Shader export has no build-bound compiler receipt.");
		}

		const auto compiler = ReadReceipt(compilerPath, {"revision", "sourceInputsSha256", "dirtyPatchSha256", "product", "variant", "executableSha256"});
		for (const auto key : {"revision", "sourceInputsSha256", "dirtyPatchSha256"})
		{
			const auto& identity = compiler.at(key).get_ref<const std::string&>();
			if (identity.empty() || identity != product.at(key).get_ref<const std::string&>())
			{
				throw Diagnostics::Error("Shader export and product belong to different source candidates.");
			}
		}

		const bool bCompilerMatches = compiler.at("product").get_ref<const std::string&>() == "ShaderCompiler";
		const bool bVariantMatches = bCompilerMatches && compiler.at("variant").get_ref<const std::string&>() == "gpu-crash";
		const auto& compilerHash = compiler.at("executableSha256").get_ref<const std::string&>();
		const bool bCompilerIdentity = bVariantMatches && !compilerHash.empty();
		const bool bCompilerHashMatches = bCompilerIdentity && compilerHash == bundle.at("shaderCompilerExecutableSha256").get_ref<const std::string&>();
		if (!bCompilerHashMatches)
		{
			throw Diagnostics::Error("Shader export does not match the active cooked publication.");
		}

		const auto& expectedMapHash = bundle.at("globalShaderMapSha256").get_ref<const std::string&>();
		const auto& mapPath = Filesystem::GetGlobalShaderMapPath();
		const std::string mapHash = HashFile(mapPath);
		if (expectedMapHash != mapHash)
		{
			throw Diagnostics::Error("Shader export does not match the active cooked publication.");
		}

		const auto& expectedLibraryHash = bundle.at("cookedShaderLibrarySha256").get_ref<const std::string&>();
		const auto& libraryPath = Filesystem::GetCookedShaderLibraryPath();
		const std::string libraryHash = HashFile(libraryPath);
		if (expectedLibraryHash != libraryHash)
		{
			throw Diagnostics::Error("Shader export does not match the active cooked publication.");
		}
		VerifyShaderJobs(api, bundleRoot, bundle, files);
		return bundle;
	}
}
