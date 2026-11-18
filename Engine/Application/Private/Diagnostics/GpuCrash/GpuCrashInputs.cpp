#include "PCH.h"
#include "Core/Public/Paths/CookedAssetPaths.h"
#include "Diagnostics/GpuCrash/GpuCrashInputs.h"

#include "Diagnostics/GpuCrash/GpuCrashInputVerification.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Strings/StringUtils.h"

namespace GpuCrashInputVerification
{
	static nlohmann::json VerifyProductReceipt(const std::filesystem::path& executable)
	{
		std::string productBytes;

		const auto product = ReadReceipt(
		    executable.parent_path() / "gpu-crash-product.json",
		    {"profile",
		        "revision",
		        "sourceInputsSha256",
		        "dirtyPatchSha256",
		        "variant",
		        "product",
		        "executableSha256",
		        "retainedReceiptPath",
		        "runtimeFiles",
		        "cpuPdbPath",
		        "cpuPdbSha256",
		        "cpuPdbGuid",
		        "cpuPdbAge"},
		    &productBytes);

		const auto profile = product.at("profile").get<std::string>();
		for (const auto key : {"revision", "sourceInputsSha256", "dirtyPatchSha256"})
		{
			const auto value = product.at(key).get<std::string>();
			const auto expectedDigestLength = std::string_view(key) == "revision" ? 40u : 64u;
			const bool bInvalidCandidateDigest = value.size() != expectedDigestLength || value.find_first_not_of("0123456789abcdef") != std::string::npos;

			if (bInvalidCandidateDigest)
			{
				throw Diagnostics::Error("Diagnostic source candidate identity is malformed.");
			}
		}

		const bool bProfileKnownProfile = profile == "DevelopmentEditor" || profile == "DevelopmentGame";
		const bool bKnownEditorProfile = bProfileKnownProfile || (profile == "ShippingEditor");
		const bool bProfileKnown = bKnownEditorProfile || (profile == "ShippingGame");
		const bool bDirectoryMatches = bProfileKnown && profile == Strings::ToNarrow(executable.parent_path().filename().wstring());
		const bool bVariantMatches = bDirectoryMatches && product.at("variant").get<std::string>() == "gpu-crash";
		const bool bProductKindMatches = bVariantMatches && Strings::ToNarrow(executable.stem().wstring()).ends_with("Editor") == profile.ends_with("Editor");
		const bool bProductNameMatches = bProductKindMatches && product.at("product").get<std::string>() == Strings::ToNarrow(executable.stem().wstring());
		const bool bExecutableMatches = bProductNameMatches && product.at("executableSha256").get<std::string>() == HashFile(executable);

		if (!bExecutableMatches)
		{
			throw Diagnostics::Error("GPU crash product/profile receipt does not match this executable.");
		}

		const std::filesystem::path retainedReceipt = Strings::ToWide(product.at("retainedReceiptPath").get<std::string>());
		CheckLocalPath(retainedReceipt);
		std::string retainedBytes;
		std::string error;
		const bool bRetainedRead = Files::TryReadAllText(retainedReceipt, retainedBytes, error, 32u * 1024u * 1024u);
		if (!bRetainedRead || retainedBytes != productBytes)
		{
			throw Diagnostics::Error("Retained product receipt differs from this build's startup receipt.");
		}
		VerifyReceiptFiles(executable.parent_path(), product.at("runtimeFiles"), false);
		const std::filesystem::path pdb = Strings::ToWide(product.at("cpuPdbPath").get<std::string>());
		if (HashFile(pdb) != product.at("cpuPdbSha256").get<std::string>())
		{
			throw Diagnostics::Error("Matching CPU symbols are missing or changed.");
		}
		return product;
	}

}

static nlohmann::json VerifyGpuCrashInputs(const GpuCrashStartup& startup, ERhiBackendApi api)
{
	using namespace GpuCrashInputVerification;
	const auto executable = Filesystem::GetExecutablePath();
	const auto product = VerifyProductReceipt(executable);
	const std::filesystem::path productPath = Strings::ToWide(product.at("retainedReceiptPath").get<std::string>());
	const auto bundle = VerifyShaderBundle(startup.ShaderExport, product, api);
	const std::filesystem::path pdb = Strings::ToWide(product.at("cpuPdbPath").get<std::string>());
	nlohmann::json identity = nlohmann::json::object();
	for (const auto key : {"product", "profile", "revision", "sourceInputsSha256", "dirtyPatchSha256", "cpuPdbGuid", "cpuPdbSha256"})
	{
		const auto& value = product.at(key).get_ref<const std::string&>();
		if (value.empty())
		{
			throw Diagnostics::Error("Diagnostic candidate identity strings must be nonempty.");
		}
		identity[key] = value;
	}
	const auto& age = product.at("cpuPdbAge");
	if (!age.is_number_unsigned())
	{
		throw Diagnostics::Error("CPU PDB age is unavailable.");
	}
	identity["cpuPdbAge"] = age;
	identity["graphicsApi"] = RhiBackendApiToString(api);
	identity["productReceiptSha256"] = HashFile(productPath);
	identity["shaderExportSha256"] = HashFile(startup.ShaderExport);
	identity["globalShaderMapSha256"] = bundle.at("globalShaderMapSha256").get<std::string>();
	identity["cookedShaderLibrarySha256"] = bundle.at("cookedShaderLibrarySha256").get<std::string>();
	identity["aftermathRuntimeSha256"] = HashFile(ResolveAftermathLibraryPath());
	// These references are private, never projected into the default export.
	identity["shaderExportPath"] = Strings::ToNarrow(startup.ShaderExport.wstring());
	identity["productReceiptPath"] = Strings::ToNarrow(productPath.wstring());
	identity["cpuPdbPath"] = Strings::ToNarrow(pdb.wstring());
	return identity;
}

GpuCrashInputs::GpuCrashInputs(const GpuCrashStartup& startup, ERhiBackendApi api)
{
	try
	{
		const std::array paths{Filesystem::GetGlobalShaderMapPath(), Filesystem::GetCookedShaderLibraryPath()};
		for (std::size_t index = 0; index < paths.size(); ++index)
		{
			GpuCrashInputVerification::CheckLocalPath(paths[index]);
			m_cookedFiles[index] = CreateFileW(paths[index].c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (m_cookedFiles[index] == INVALID_HANDLE_VALUE)
			{
				throw Diagnostics::Error("Cannot freeze the diagnostic cooked shader publication; close its writer and retry.");
			}
		}
		m_identity = VerifyGpuCrashInputs(startup, api);
	}
	catch (...)
	{
		for (const auto file : m_cookedFiles)
		{
			if (file != INVALID_HANDLE_VALUE)
			{
				CloseHandle(file);
			}
		}
		throw;
	}
}

GpuCrashInputs::~GpuCrashInputs()
{
	for (const auto file : m_cookedFiles)
	{
		CloseHandle(file);
	}
}

const nlohmann::json& GpuCrashInputs::GetIdentity() const noexcept
{
	return m_identity;
}

std::filesystem::path ResolveAftermathLibraryPath()
{
	// Both local and staged products use Projects/<project>/Binaries/<profile>.
	const auto directory = Filesystem::GetExecutableDirectory();
	const auto productRoot = directory.parent_path().parent_path().parent_path().parent_path();
	const bool bInvalidProductLocation = directory.parent_path().filename() != "Binaries" || directory.parent_path().parent_path().parent_path().filename() != "Projects";

	if (bInvalidProductLocation)
	{
		throw Diagnostics::Error("GPU crash startup requires the product Binaries layout.");
	}
	return productRoot / "Engine/Binaries/Aftermath" / directory.filename() / "GFSDK_Aftermath_Lib.x64.dll";
}
