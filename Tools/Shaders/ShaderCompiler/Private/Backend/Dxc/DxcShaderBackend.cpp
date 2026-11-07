#include "PCH.h"

#include "DxcShaderBackend.h"
#include "DxcCompileArguments.h"

#include "Compiler/SpirV/SpirVBindingNormalizer.h"
#include "Compiler/SpirV/SpirVDisassembler.h"

#include "Compiler/ShaderCompileProfile.h"
#include "Constants/ShaderCompilerConstants.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"
#include "DxilReflectionExtractor.h"
#include "Compiler/SpirV/SpirVReflectionExtractor.h"

#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_dxcShaderBackendLogger, kDxcCompilerLoggerCategory);

DxcShaderBackend::DxcShaderBackend()
{
	HRESULT hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(m_compiler.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		Diagnostics::Fatal(g_dxcShaderBackendLogger, __FILE__, __LINE__, "Failed to create DXC compiler instance");
		return;
	}

	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(m_utils.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		Diagnostics::Fatal(g_dxcShaderBackendLogger, __FILE__, __LINE__, "Failed to create DXC utils instance");
		m_compiler.Reset();
		return;
	}

	m_backendVersion = QueryBackendVersion(*m_compiler.Get());
}

ShaderBackendCapabilities DxcShaderBackend::GetStaticCapabilities() noexcept
{
	ShaderBackendCapabilities capabilities;
	capabilities.SupportsDxil = true;
	capabilities.SupportsSpirV = true;
	capabilities.SupportsDxilRayTracingLibrary = true;
	capabilities.SupportsSpirVRayTracingLibrary = true;
	capabilities.SupportsDxilInlineRayQuery = true;
	capabilities.SupportsSpirVInlineRayQuery = true;
	return capabilities;
}

std::uint64_t DxcShaderBackend::QueryBackendVersion()
{
	Microsoft::WRL::ComPtr<IDxcCompiler3> compiler;
	if (FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler.ReleaseAndGetAddressOf()))) || !compiler)
	{
		return 0;
	}
	return QueryBackendVersion(*compiler.Get());
}

std::uint64_t DxcShaderBackend::QueryBackendVersion(IDxcCompiler3& compiler)
{
	Microsoft::WRL::ComPtr<IDxcVersionInfo> versionInfo;
	if (FAILED(compiler.QueryInterface(IID_PPV_ARGS(versionInfo.ReleaseAndGetAddressOf()))) || !versionInfo)
	{
		return 0;
	}

	UINT32 major = 0;
	UINT32 minor = 0;
	versionInfo->GetVersion(&major, &minor);
	UINT32 commit = 0;
	Microsoft::WRL::ComPtr<IDxcVersionInfo2> versionInfo2;
	if (SUCCEEDED(versionInfo.As(&versionInfo2)))
	{
		char* commitHash = nullptr;
		versionInfo2->GetCommitInfo(&commit, &commitHash);
		if (commitHash != nullptr)
		{
			CoTaskMemFree(commitHash);
		}
	}
	return (static_cast<std::uint64_t>(major) << 48u) | (static_cast<std::uint64_t>(minor) << 32u) | static_cast<std::uint64_t>(commit);
}

ShaderBackendCapabilities DxcShaderBackend::GetCapabilities() const
{
	ShaderBackendCapabilities capabilities = GetStaticCapabilities();
	capabilities.SupportsDxil = capabilities.SupportsDxil && IsValid();
	capabilities.SupportsSpirV = capabilities.SupportsSpirV && IsValid();
	capabilities.SupportsDxilRayTracingLibrary = capabilities.SupportsDxilRayTracingLibrary && IsValid();
	capabilities.SupportsSpirVRayTracingLibrary = capabilities.SupportsSpirVRayTracingLibrary && IsValid();
	capabilities.SupportsDxilInlineRayQuery = capabilities.SupportsDxilInlineRayQuery && IsValid();
	capabilities.SupportsSpirVInlineRayQuery = capabilities.SupportsSpirVInlineRayQuery && IsValid();
	return capabilities;
}

std::string_view DxcShaderBackend::GetBackendName() const
{
	return "dxc";
}

std::uint64_t DxcShaderBackend::GetBackendVersion() const
{
	return m_backendVersion;
}

CompiledShader DxcShaderBackend::Compile(const ShaderCompileRequest& request)
{
	if (!IsValid())
	{
		throw Diagnostics::Error("DXC backend is not initialized");
	}

	const std::string& sourceText = request.SourceCode;

	DxcBuffer sourceBuffer{};
	sourceBuffer.Ptr = sourceText.data();
	sourceBuffer.Size = sourceText.size();
	sourceBuffer.Encoding = DXC_CP_UTF8;

	std::wstring wSourcePath = Strings::ToWide(std::string_view{request.VirtualSourcePath});
	std::wstring wEntryPoint = Strings::ToWide(std::string_view{request.EntryPoint});
	std::wstring wTargetProfile = Strings::ToWide(std::string_view{BuildShaderTargetProfile(request)});
	std::vector<std::wstring> wDefines;
	std::vector<LPCWSTR> args;

	BuildDxcCompileArguments(request, wSourcePath, wEntryPoint, wTargetProfile, wDefines, args);

	std::string compilerOutput;
	const Microsoft::WRL::ComPtr<IDxcResult> result = CompileSource(request, sourceBuffer, args, compilerOutput);

	std::vector<std::uint8_t> bytecode = ExtractBytecode(result.Get());
	if (bytecode.empty())
	{
		throw Diagnostics::Error("Failed to extract shader bytecode");
	}
	if (IsSpirVTarget(request.Target))
	{
		NormalizeSpirVBindings(bytecode, request.DescriptorBindingRemaps);
	}

	ShaderReflection reflection = IsSpirVTarget(request.Target) ? ExtractSpirVReflection(bytecode, request.Stage)
	    : request.UnitKind == ShaderCompileUnitKind::Library    ? ExtractDxilLibraryReflection(*m_utils.Get(), result.Get(), request.EntryPoint)
	                                                            : ExtractDxilReflection(*m_utils.Get(), result.Get(), request.Stage);

	ShaderDebugArtifactSet debugArtifacts;
	if (request.CaptureDebugArtifacts)
	{
		debugArtifacts = CaptureDebugArtifacts(request, *result.Get(), sourceBuffer, bytecode, args, compilerOutput);
	}

	CompiledShader compiledShader(std::move(bytecode));
	compiledShader.SetReflection(std::move(reflection));
	compiledShader.SetDebugArtifacts(std::move(debugArtifacts));
	return compiledShader;
}

Microsoft::WRL::ComPtr<IDxcResult> DxcShaderBackend::CompileSource(const ShaderCompileRequest& request, const DxcBuffer& sourceBuffer, std::span<LPCWSTR> args, std::string& compilerOutput)
{
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;
	if (!request.CaptureDebugArtifacts)
	{
		m_utils->CreateDefaultIncludeHandler(includeHandler.ReleaseAndGetAddressOf());
	}

	Microsoft::WRL::ComPtr<IDxcResult> result;
	HRESULT hr = m_compiler->Compile(&sourceBuffer, args.data(), static_cast<UINT>(args.size()), includeHandler.Get(), IID_PPV_ARGS(result.ReleaseAndGetAddressOf()));

	if (FAILED(hr) || !result)
	{
		throw Diagnostics::Error("DXC Compile() call failed");
	}

	compilerOutput = ExtractErrorMessage(result.Get());

	HRESULT status;
	result->GetStatus(&status);
	if (FAILED(status))
	{
		if (compilerOutput.empty())
		{
			compilerOutput = "Compilation failed with no error message";
		}

		SPDLOG_LOGGER_ERROR(
		    g_dxcShaderBackendLogger,
		    "Shader compilation failed for type '{}' source '{}' entry '{}' target '{}': {}",
		    request.ShaderTypeName,
		    request.VirtualSourcePath,
		    request.EntryPoint,
		    GetShaderTargetName(request.Target),
		    compilerOutput);

		throw Diagnostics::Error(compilerOutput);
	}

	if (!compilerOutput.empty())
	{
		SPDLOG_LOGGER_WARN(g_dxcShaderBackendLogger, "Shader warnings: {}", compilerOutput);
	}

	return result;
}

std::vector<std::uint8_t> DxcShaderBackend::ExtractBytecode(IDxcResult* result)
{
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob;
	HRESULT hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(shaderBlob.ReleaseAndGetAddressOf()), nullptr);
	if (FAILED(hr) || !shaderBlob || shaderBlob->GetBufferSize() == 0)
	{
		return {};
	}

	const std::uint8_t* data = static_cast<const std::uint8_t*>(shaderBlob->GetBufferPointer());
	return std::vector<std::uint8_t>(data, data + shaderBlob->GetBufferSize());
}

std::string DxcShaderBackend::ExtractErrorMessage(IDxcResult* result)
{
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> errorBlob;
	result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errorBlob.ReleaseAndGetAddressOf()), nullptr);
	if (errorBlob && errorBlob->GetStringLength() > 0)
	{
		return std::string(errorBlob->GetStringPointer(), errorBlob->GetStringLength());
	}
	return {};
}

std::string DxcShaderBackend::ExtractTextOutput(IDxcResult* result, DXC_OUT_KIND kind)
{
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> textBlob;
	if (SUCCEEDED(result->GetOutput(kind, IID_PPV_ARGS(textBlob.ReleaseAndGetAddressOf()), nullptr)) && textBlob)
	{
		return std::string(textBlob->GetStringPointer(), textBlob->GetStringLength());
	}

	Microsoft::WRL::ComPtr<IDxcBlobWide> wideBlob;
	if (SUCCEEDED(result->GetOutput(kind, IID_PPV_ARGS(wideBlob.ReleaseAndGetAddressOf()), nullptr)) && wideBlob)
	{
		return Strings::ToNarrow(std::wstring_view(wideBlob->GetStringPointer(), wideBlob->GetStringLength()));
	}

	return {};
}

std::string DxcShaderBackend::ExtractPreprocessedSource(IDxcCompiler3& compiler, const DxcBuffer& sourceBuffer, const std::vector<LPCWSTR>& compileArgs)
{
	std::vector<LPCWSTR> preprocessArgs = compileArgs;
	preprocessArgs.push_back(L"-P");

	Microsoft::WRL::ComPtr<IDxcResult> preprocessResult;
	const HRESULT preprocessStatus = compiler.Compile(&sourceBuffer, preprocessArgs.data(), static_cast<UINT>(preprocessArgs.size()), nullptr, IID_PPV_ARGS(preprocessResult.ReleaseAndGetAddressOf()));

	if (FAILED(preprocessStatus) || !preprocessResult)
	{
		return {};
	}

	std::string preprocessed = ExtractTextOutput(preprocessResult.Get(), DXC_OUT_HLSL);
	if (!preprocessed.empty())
	{
		return preprocessed;
	}

	return {};
}

std::string DxcShaderBackend::ExtractDisassembly(IDxcUtils& utils, IDxcCompiler3& compiler, std::span<const std::uint8_t> bytecode)
{
	if (bytecode.empty())
	{
		return {};
	}

	DxcBuffer objectBuffer{};
	objectBuffer.Ptr = bytecode.data();
	objectBuffer.Size = bytecode.size();
	objectBuffer.Encoding = 0;

	Microsoft::WRL::ComPtr<IDxcResult> disassemblyResult;
	const HRESULT disassemblyStatus = compiler.Disassemble(&objectBuffer, IID_PPV_ARGS(disassemblyResult.ReleaseAndGetAddressOf()));
	const bool disassemblyAvailable = SUCCEEDED(disassemblyStatus) && disassemblyResult;
	
	if (!disassemblyAvailable)
	{
		return {};
	}

	std::string disassembly = ExtractTextOutput(disassemblyResult.Get(), DXC_OUT_DISASSEMBLY);
	if (!disassembly.empty())
	{
		return disassembly;
	}

	Microsoft::WRL::ComPtr<IDxcCompiler> disassemblyCompiler;
	if (FAILED(compiler.QueryInterface(IID_PPV_ARGS(disassemblyCompiler.ReleaseAndGetAddressOf()))) || !disassemblyCompiler)
	{
		return {};
	}

	Microsoft::WRL::ComPtr<IDxcBlobEncoding> objectBlob;
	const HRESULT blobStatus = utils.CreateBlobFromPinned(bytecode.data(), static_cast<UINT32>(bytecode.size()), 0, objectBlob.ReleaseAndGetAddressOf());

	if (FAILED(blobStatus) || !objectBlob)
	{
		return {};
	}

	Microsoft::WRL::ComPtr<IDxcBlobEncoding> disassemblyBlob;
	if (FAILED(disassemblyCompiler->Disassemble(objectBlob.Get(), disassemblyBlob.ReleaseAndGetAddressOf())) || !disassemblyBlob)
	{
		return {};
	}

	return std::string(static_cast<const char*>(disassemblyBlob->GetBufferPointer()), static_cast<std::size_t>(disassemblyBlob->GetBufferSize()));
}

ShaderDebugArtifactSet DxcShaderBackend::CaptureDebugArtifacts(
    const ShaderCompileRequest& request,
    IDxcResult& result,
    const DxcBuffer& sourceBuffer,
    std::span<const std::uint8_t> bytecode,
    const std::vector<LPCWSTR>& compileArgs,
    std::string_view compilerOutput) const
{
	ShaderDebugArtifactSet debugArtifacts;
	debugArtifacts.CompileArguments = BuildDebugArgumentStrings(compileArgs);
	debugArtifacts.CompilerOutput.assign(compilerOutput);
	debugArtifacts.Disassembly = IsSpirVTarget(request.Target) ? DisassembleSpirV(bytecode) : ExtractDisassembly(*m_utils.Get(), *m_compiler.Get(), bytecode);
	if (debugArtifacts.Disassembly.empty())
	{
		throw Diagnostics::Error("DXC failed to capture disassembly for shader source '" + request.VirtualSourcePath + "'.");
	}
	debugArtifacts.PreprocessedSource = ExtractPreprocessedSource(*m_compiler.Get(), sourceBuffer, compileArgs);
	if (debugArtifacts.PreprocessedSource.empty())
	{
		throw Diagnostics::Error("DXC failed to capture preprocessed source for shader source '" + request.VirtualSourcePath + "'.");
	}
	if (!IsSpirVTarget(request.Target))
	{
		ExtractSymbols(result, debugArtifacts);
	}

	return debugArtifacts;
}

std::vector<std::string> DxcShaderBackend::BuildDebugArgumentStrings(const std::vector<LPCWSTR>& compileArgs)
{
	std::vector<std::string> args;
	args.reserve(compileArgs.size());
	for (const LPCWSTR arg : compileArgs)
	{
		args.push_back(Strings::ToNarrow(std::wstring_view(arg)));
	}
	return args;
}

void DxcShaderBackend::ExtractSymbols(IDxcResult& result, ShaderDebugArtifactSet& artifacts)
{
	Microsoft::WRL::ComPtr<IDxcBlob> pdbBlob;
	Microsoft::WRL::ComPtr<IDxcBlobUtf16> pdbNameBlob;
	const HRESULT symbolStatus = result.GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(pdbBlob.ReleaseAndGetAddressOf()), pdbNameBlob.ReleaseAndGetAddressOf());

	if (FAILED(symbolStatus) || !pdbBlob || pdbBlob->GetBufferSize() == 0 || !pdbNameBlob || pdbNameBlob->GetStringLength() == 0)
	{
		throw Diagnostics::Error("DXC did not return the requested full shader PDB and debug name.");
	}

	const auto* pdbBytes = static_cast<const std::uint8_t*>(pdbBlob->GetBufferPointer());
	artifacts.Symbols.assign(pdbBytes, pdbBytes + pdbBlob->GetBufferSize());
	artifacts.SymbolName = Strings::ToNarrow(std::wstring_view(pdbNameBlob->GetStringPointer(), pdbNameBlob->GetStringLength()));
	Microsoft::WRL::ComPtr<IDxcBlob> hashBlob;
	const HRESULT hashStatus = result.GetOutput(DXC_OUT_SHADER_HASH, IID_PPV_ARGS(hashBlob.ReleaseAndGetAddressOf()), nullptr);

	if (FAILED(hashStatus) || !hashBlob || hashBlob->GetBufferSize() != sizeof(DxcShaderHash))
	{
		throw Diagnostics::Error("DXC did not return the requested compiler shader hash.");
	}

	const auto* hashBytes = static_cast<const std::uint8_t*>(hashBlob->GetBufferPointer());
	artifacts.CompilerLookupKey.assign(hashBytes, hashBytes + hashBlob->GetBufferSize());
}
