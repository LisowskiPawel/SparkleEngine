#include "PCH.h"

#include "Cooking/Compilation/ShaderMake/ShaderMakeOutputs.h"
#include "Cooking/Compilation/ShaderMake/ShaderMakeProducer.h"
#include "Compiler/Dxil/DxilDebugArtifacts.h"
#include "Compiler/Dxil/DxilReflectionExtractor.h"
#include "Compiler/Slang/SlangReflection.h"
#include "Compiler/SpirV/SpirVBindingNormalizer.h"
#include "Compiler/SpirV/SpirVDisassembler.h"
#include "Compiler/SpirV/SpirVReflectionExtractor.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Hash/HashUtils.h"

#include <Windows.h>
#include <wrl/client.h>
#include <algorithm>
#include <cstring>
#include <format>
#include <string>

static constexpr std::size_t kMaximumCompilerArtifactBytes = 64u * 1024u * 1024u;

static void RejectReparsePoint(const std::filesystem::path& path)
{
	const DWORD attributes = GetFileAttributesW(path.c_str());
	const bool bMissing = attributes == INVALID_FILE_ATTRIBUTES;
	const bool bReparse = (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
	if (bMissing || bReparse)
	{
		throw Diagnostics::Error("ShaderMake output is missing or traverses a reparse point.");
	}
}

void ValidateShaderMakeTree(const std::filesystem::path& root)
{
	RejectReparsePoint(root);
	std::size_t members = 0;
	for (std::filesystem::recursive_directory_iterator entry(root), end; entry != end; ++entry)
	{
		RejectReparsePoint(entry->path());
		if (++members > 128u || entry.depth() > 8)
		{
			throw Diagnostics::Error("ShaderMake producer output exceeds its member/depth limit.");
		}
		if (entry->is_regular_file() && entry->file_size() > kMaximumCompilerArtifactBytes)
		{
			throw Diagnostics::Error("ShaderMake producer output exceeds its byte limit.");
		}
	}
}

static std::vector<std::uint8_t> ReadCompilerBytes(const std::filesystem::path& path)
{
	RejectReparsePoint(path);
	std::vector<std::uint8_t> bytes;
	std::string error;
	if (!Files::TryReadAllBytes(path, bytes, error, kMaximumCompilerArtifactBytes))
	{
		throw Diagnostics::Error(error);
	}
	if (bytes.empty())
	{
		throw Diagnostics::Error("ShaderMake emitted an empty required artifact.");
	}
	return bytes;
}

static void ValidateTarget(IDxcUtils& utils, const ShaderCompileRequest& request, std::span<const std::uint8_t> code)
{
	if (IsSpirVTarget(request.Target))
	{
		std::uint32_t header[2]{};
		if (code.size() < sizeof(header))
		{
			throw Diagnostics::Error("ShaderMake emitted a truncated SPIR-V header.");
		}
		std::memcpy(header, code.data(), sizeof(header));
		const std::uint32_t minor = request.Target == ShaderTarget::SpirV14 ? 4u : request.Target == ShaderTarget::SpirV15 ? 5u : 6u;
		if (header[0] != 0x07230203u || header[1] != (0x10000u | (minor << 8u)))
		{
			throw Diagnostics::Error("ShaderMake emitted the wrong SPIR-V target version.");
		}
		return;
	}
	const DxcBuffer buffer{.Ptr = code.data(), .Size = code.size(), .Encoding = 0};
	void* program = nullptr;
	UINT32 size = 0;
	const HRESULT status = utils.GetDxilContainerPart(&buffer, DXC_PART_DXIL, &program, &size);
	const bool bPartAvailable = SUCCEEDED(status) && program;
	if (!bPartAvailable || size < sizeof(std::uint32_t))
	{
		throw Diagnostics::Error("ShaderMake emitted invalid DXIL code.");
	}
	std::uint32_t version = 0;
	std::memcpy(&version, program, sizeof(version));
	const auto targetMinor = static_cast<std::uint32_t>(std::string_view(GetShaderTargetName(request.Target)).back() - '0');
	const std::uint32_t kind = request.UnitKind == ShaderCompileUnitKind::Library ? 6u
	    : request.Stage == ShaderStage::Vertex                                    ? 1u
	    : request.Stage == ShaderStage::Pixel                                     ? 0u
	    : request.Stage == ShaderStage::Geometry                                  ? 2u
	    : request.Stage == ShaderStage::Hull                                      ? 3u
	    : request.Stage == ShaderStage::Domain                                    ? 4u
	                                                                              : 5u;
	if (version != ((kind << 16u) | 0x60u | targetMinor))
	{
		throw Diagnostics::Error("ShaderMake emitted the wrong DXIL stage or shader model.");
	}
}

ShaderCompileResult ReadShaderMakeOutput(const std::filesystem::path& root, const ShaderMakeProducer& producer, std::shared_ptr<const ShaderCompileInvocation> invocation)
{
	const ShaderCompileJob& job = producer.Job;
	const ShaderCompileRequest& request = job.Request;
	const bool bSlang = job.BackendName == "slang";
	const bool bDxil = IsDxilTarget(request.Target);
	const bool bContainer = bSlang && bDxil && request.bCaptureDebugArtifacts;
	const std::filesystem::path outputDirectory = root / "out" / producer.Name;
	RejectReparsePoint(root);
	RejectReparsePoint(root / "out");
	ValidateShaderMakeTree(outputDirectory);
	const std::filesystem::path codePath = root / producer.Code;
	std::vector<std::filesystem::path> codeFiles;
	std::vector<std::filesystem::path> pdbFiles;
	for (const auto& file : std::filesystem::recursive_directory_iterator(outputDirectory))
	{
		if (!file.is_regular_file())
		{
			continue;
		}
		const std::filesystem::path extension = file.path().extension();
		if (extension == ".dxil" || extension == ".spirv")
		{
			codeFiles.push_back(file.path());
		}
		const bool bAssociatedPdb = extension == ".unknown" && file.path().parent_path().extension() == ".pdb";
		if (extension == ".pdb" || bAssociatedPdb)
		{
			pdbFiles.push_back(file.path());
		}
	}
	if (codeFiles.size() != 1u || codeFiles.front() != codePath)
	{
		throw Diagnostics::Error("ShaderMake did not emit exactly the declared producer code artifact.");
	}
	std::vector<std::uint8_t> bytecode = ReadCompilerBytes(codePath);
	Microsoft::WRL::ComPtr<IDxcUtils> utils;
	const HRESULT createStatus = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.ReleaseAndGetAddressOf()));
	if (FAILED(createStatus) || !utils)
	{
		throw Diagnostics::Error("DXIL artifact reader is unavailable.");
	}
	ValidateTarget(*utils.Get(), request, bytecode);
	ShaderReflection reflection;
	if (!bDxil)
	{
		NormalizeSpirVBindings(bytecode, request.DescriptorBindingRemaps);
		reflection = ExtractSpirVReflection(bytecode, request.Stage);
	}
	else
	{
		const std::vector<std::uint8_t> metadata = ReadCompilerBytes(root / producer.Reflection);
		if (bSlang)
		{
			const std::string_view text(reinterpret_cast<const char*>(metadata.data()), metadata.size());
			reflection = ReadSlangReflection(text, request.Stage, request.EntryPoint);
		}
		else
		{
			reflection = request.UnitKind == ShaderCompileUnitKind::Library ? ExtractDxilLibraryReflection(*utils.Get(), metadata, request.EntryPoint)
			                                                                : ExtractDxilReflection(*utils.Get(), metadata, request.Stage);
		}
	}
	ShaderDebugArtifactSet debug;
	if (request.bCaptureDebugArtifacts)
	{
		debug.Invocation = std::move(invocation);
		debug.CompilerInputPath = producer.Input;
		debug.Disassembly = bDxil ? DisassembleDxil(bytecode) : DisassembleSpirV(bytecode);
		if (bSlang)
		{
			debug.PreprocessedSource = request.SourceCode;
		}
		else
		{
			RejectReparsePoint(root / "preprocessed");
			ValidateShaderMakeTree((root / producer.Preprocessed).parent_path());
			const auto text = ReadCompilerBytes(root / producer.Preprocessed);
			debug.PreprocessedSource.assign(text.begin(), text.end());
		}
		if (bDxil)
		{
			if (pdbFiles.size() != 1u)
			{
				throw Diagnostics::Error("ShaderMake did not provide exactly one full shader PDB; verify the SDK debug support and its Windows path-length limit.");
			}
			debug.Symbols = ReadCompilerBytes(pdbFiles.front());
			debug.SymbolName = ReadDxilSymbolName(*utils.Get(), bytecode, debug.Symbols);
			const std::filesystem::path expectedParent = root / producer.Symbols;
			const std::filesystem::path actualParent = bContainer ? pdbFiles.front().parent_path().parent_path() : pdbFiles.front().parent_path();
			if (actualParent != expectedParent)
			{
				throw Diagnostics::Error("Shader PDB is outside its declared code association.");
			}
			const std::string nativeName = bContainer ? pdbFiles.front().parent_path().filename().string() : pdbFiles.front().filename().string();
			if (nativeName != debug.SymbolName)
			{
				throw Diagnostics::Error("Shader PDB path disagrees with its compiler name.");
			}
		}
	}
	const std::uint64_t bytecodeHash = Hash::Fnv1a64(bytecode.data(), bytecode.size());
	CookedStageBuild output{
	    .stage = request.Stage,
	    .format = bDxil ? ShaderBinaryFormat::Dxil : ShaderBinaryFormat::SpirV,
	    .sourcePath = request.VirtualSourcePath,
	    .entryPoint = request.EntryPoint,
	    .backendName = job.BackendName,
	    .codegenTarget = GetShaderTargetName(request.Target),
	    .backendVersion = job.BackendVersion,
	    .sourceHash = job.SourceContentHash,
	    .includeClosureHash = job.DependencyClosureHash,
	    .requestHash = job.RequestHash,
	    .compileInputHash = job.InputHash,
	    .bytecode = std::move(bytecode),
	    .bytecodeHash = bytecodeHash,
	    .reflection = std::move(reflection)};
	return {.ShaderType = request.ShaderType, .Target = request.Target, .InputHash = job.InputHash, .Output = std::move(output), .DebugArtifacts = std::move(debug)};
}
