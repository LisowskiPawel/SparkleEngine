#include "PCH.h"
#include "Diagnostics/GpuCrash/GpuCrashInputs.h"

#include "Diagnostics/GpuCrash/GpuCrashInputVerification.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Json/JsonReader.h"
#include "Core/Public/Json/JsonWriter.h"
#include "Core/Public/Strings/StringUtils.h"

namespace GpuCrashInputVerification
{
	static std::string VerifyProductReceipt(const std::filesystem::path& executable)
	{
		const auto product = ReadReceipt(executable.parent_path() / "gpu-crash-product.json");
		const auto profile = ReadRequiredString(product, "profile");
		for (const auto key : {"revision", "sourceInputsSha256", "dirtyPatchSha256"})
		{
			const auto value = ReadRequiredString(product, key);
			if (value.size() != (std::string_view(key) == "revision" ? 40 : 64) || value.find_first_not_of("0123456789abcdef") != std::string::npos)
			{
				throw Diagnostics::Error("Diagnostic source candidate identity is malformed.");
			}
		}
		if ((profile != "DevelopmentEditor" && profile != "DevelopmentGame" && profile != "ShippingEditor" && profile != "ShippingGame")
		    || profile != Strings::ToNarrow(executable.parent_path().filename().wstring()) || ReadRequiredString(product, "variant") != "gpu-crash"
		    || Strings::ToNarrow(executable.stem().wstring()).ends_with("Editor") != profile.ends_with("Editor")
		    || ReadRequiredString(product, "product") != Strings::ToNarrow(executable.stem().wstring()) || ReadRequiredString(product, "executableSha256") != HashFile(executable))
		{
			throw Diagnostics::Error("GPU crash product/profile receipt does not match this executable.");
		}
		const std::filesystem::path retainedReceipt = Strings::ToWide(std::string_view{ReadRequiredString(product, "retainedReceiptPath")});
		if (ReadReceipt(retainedReceipt) != product)
		{
			throw Diagnostics::Error("Retained product receipt differs from this build's startup receipt.");
		}
		VerifyReceiptFiles(executable.parent_path(), product, "runtimeFiles");
		const std::filesystem::path pdb = Strings::ToWide(std::string_view{ReadRequiredString(product, "cpuPdbPath")});
		if (HashFile(pdb) != ReadRequiredString(product, "cpuPdbSha256"))
		{
			throw Diagnostics::Error("Matching CPU symbols are missing or changed.");
		}
		return product;
	}

}

static std::string VerifyGpuCrashInputs(const GpuCrashStartup& startup, ERhiBackendApi api)
{
	using namespace GpuCrashInputVerification;
	const auto executable = Filesystem::GetExecutablePath();
	const auto product = VerifyProductReceipt(executable);
	const std::filesystem::path productPath = Strings::ToWide(std::string_view{ReadRequiredString(product, "retainedReceiptPath")});
	const auto bundle = VerifyShaderBundle(startup.ShaderExport, product, api);
	const std::filesystem::path pdb = Strings::ToWide(std::string_view{ReadRequiredString(product, "cpuPdbPath")});
	Json::ObjectWriter identity;
	for (const auto key : {"product", "profile", "revision", "sourceInputsSha256", "dirtyPatchSha256", "cpuPdbGuid", "cpuPdbSha256"})
	{
		identity.WriteString(key, ReadRequiredString(product, key));
	}
	std::uint64_t age = 0;
	if (!Json::TryReadUInt64Property(product, "cpuPdbAge", age))
	{
		throw Diagnostics::Error("CPU PDB age is unavailable.");
	}
	identity.WriteUInt64("cpuPdbAge", age);
	identity.WriteString("graphicsApi", RhiBackendApiToString(api));
	identity.WriteString("productReceiptSha256", HashFile(productPath));
	identity.WriteString("shaderExportSha256", HashFile(startup.ShaderExport));
	identity.WriteString("globalShaderMapSha256", ReadRequiredString(bundle, "globalShaderMapSha256"));
	identity.WriteString("cookedShaderLibrarySha256", ReadRequiredString(bundle, "cookedShaderLibrarySha256"));
	identity.WriteString("aftermathRuntimeSha256", HashFile(ResolveAftermathLibraryPath()));
	// These references are private, never projected into the default export.
	identity.WriteString("shaderExportPath", Strings::ToNarrow(startup.ShaderExport.wstring()));
	identity.WriteString("productReceiptPath", Strings::ToNarrow(productPath.wstring()));
	identity.WriteString("cpuPdbPath", Strings::ToNarrow(pdb.wstring()));
	return identity.Finish();
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

const std::string& GpuCrashInputs::GetIdentity() const noexcept
{
	return m_identity;
}

std::filesystem::path ResolveAftermathLibraryPath()
{
	// Both local and staged products use Projects/<project>/Binaries/<profile>.
	const auto directory = Filesystem::GetExecutableDirectory();
	const auto productRoot = directory.parent_path().parent_path().parent_path().parent_path();
	if (directory.parent_path().filename() != "Binaries" || directory.parent_path().parent_path().parent_path().filename() != "Projects")
	{
		throw Diagnostics::Error("GPU crash startup requires the product Binaries layout.");
	}
	return productRoot / "Engine/Binaries/Aftermath" / directory.filename() / "GFSDK_Aftermath_Lib.x64.dll";
}
