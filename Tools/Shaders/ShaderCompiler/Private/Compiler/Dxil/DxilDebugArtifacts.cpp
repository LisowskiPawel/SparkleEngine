#include "PCH.h"

#include "Compiler/Dxil/DxilDebugArtifacts.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"

#include <Windows.h>
#include <ObjIdl.h>
#include <Unknwn.h>
#include <dxcapi.h>
#include <wrl/client.h>
#include <cstring>
#include <filesystem>

std::string ReadDxilSymbolName(IDxcUtils& utils, std::span<const std::uint8_t> code, std::span<const std::uint8_t> symbols)
{
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> pdbBlob;
	const HRESULT blobStatus = utils.CreateBlobFromPinned(symbols.data(), static_cast<UINT32>(symbols.size()), 0, pdbBlob.ReleaseAndGetAddressOf());
	Microsoft::WRL::ComPtr<IDxcPdbUtils2> pdb;
	const HRESULT createStatus = DxcCreateInstance(CLSID_DxcPdbUtils, IID_PPV_ARGS(pdb.ReleaseAndGetAddressOf()));
	const bool bReaderReady = SUCCEEDED(createStatus) && pdb;
	const bool bBlobReady = SUCCEEDED(blobStatus) && pdbBlob;
	if (!bReaderReady || !bBlobReady)
	{
		throw Diagnostics::Error("Cannot open compiler PDB data.");
	}
	const HRESULT loadStatus = pdb->Load(pdbBlob.Get());
	if (FAILED(loadStatus) || !pdb->IsFullPDB())
	{
		throw Diagnostics::Error("Compiler output is not a full shader PDB.");
	}
	Microsoft::WRL::ComPtr<IDxcBlob> pdbHash;
	const HRESULT pdbHashStatus = pdb->GetHash(pdbHash.ReleaseAndGetAddressOf());
	const DxcBuffer runtime{.Ptr = code.data(), .Size = code.size(), .Encoding = 0};
	void* runtimeHash = nullptr;
	UINT32 runtimeHashSize = 0;
	const HRESULT runtimeHashStatus = utils.GetDxilContainerPart(&runtime, DXC_PART_SHADER_HASH, &runtimeHash, &runtimeHashSize);
	const bool bPdbHashReady = SUCCEEDED(pdbHashStatus) && pdbHash;
	const bool bRuntimeHashReady = SUCCEEDED(runtimeHashStatus) && runtimeHash;
	if (!bPdbHashReady || !bRuntimeHashReady)
	{
		throw Diagnostics::Error("Shader code or PDB has no compiler hash.");
	}
	const bool bSizeMatches = runtimeHashSize == pdbHash->GetBufferSize();
	if (!bSizeMatches || std::memcmp(runtimeHash, pdbHash->GetBufferPointer(), runtimeHashSize) != 0)
	{
		throw Diagnostics::Error("Shader PDB does not belong to the emitted code.");
	}
	Microsoft::WRL::ComPtr<IDxcBlobWide> name;
	const HRESULT nameStatus = pdb->GetName(name.ReleaseAndGetAddressOf());
	const bool bNameAvailable = SUCCEEDED(nameStatus) && name;
	if (!bNameAvailable || name->GetStringLength() == 0)
	{
		throw Diagnostics::Error("Shader PDB has no compiler symbol name.");
	}
	const std::wstring_view wideName(name->GetStringPointer(), name->GetStringLength());
	const std::filesystem::path filename = std::filesystem::path(wideName).filename();
	if (filename.extension() != ".pdb")
	{
		throw Diagnostics::Error("Compiler symbol name does not identify a PDB.");
	}
	return Strings::ToNarrow(filename.native());
}

std::string DisassembleDxil(std::span<const std::uint8_t> code)
{
	Microsoft::WRL::ComPtr<IDxcCompiler3> reader;
	const HRESULT createStatus = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(reader.ReleaseAndGetAddressOf()));
	if (FAILED(createStatus) || !reader)
	{
		throw Diagnostics::Error("DXIL disassembler is unavailable.");
	}
	const DxcBuffer buffer{.Ptr = code.data(), .Size = code.size(), .Encoding = 0};
	Microsoft::WRL::ComPtr<IDxcResult> result;
	const HRESULT readStatus = reader->Disassemble(&buffer, IID_PPV_ARGS(result.ReleaseAndGetAddressOf()));
	if (FAILED(readStatus) || !result)
	{
		throw Diagnostics::Error("DXIL disassembly failed.");
	}
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> text;
	const HRESULT textStatus = result->GetOutput(DXC_OUT_DISASSEMBLY, IID_PPV_ARGS(text.ReleaseAndGetAddressOf()), nullptr);
	const bool bTextAvailable = SUCCEEDED(textStatus) && text;
	if (!bTextAvailable || text->GetStringLength() == 0)
	{
		throw Diagnostics::Error("DXIL disassembly is empty.");
	}
	return {text->GetStringPointer(), text->GetStringLength()};
}
