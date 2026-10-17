#pragma once

#include "Backend/IShaderBackend.h"

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
  #define NOMINMAX
#endif
#include <cstdint>
#include <string_view>
#include <ObjIdl.h>
#include <Unknwn.h>

#include <dxcapi.h>
#include <span>
#include <string>
#include <vector>
#include <wrl/client.h>

// DXC backend used privately by GlobalShaderCooker.
// Produces both DXIL and SPIR-V via DXC's `-spirv` mode.
class DxcShaderBackend final : public IShaderBackend
{
public:
	DxcShaderBackend();
	~DxcShaderBackend() override = default;

	DxcShaderBackend(const DxcShaderBackend&) = delete;
	DxcShaderBackend& operator=(const DxcShaderBackend&) = delete;
	DxcShaderBackend(DxcShaderBackend&&) = delete;
	DxcShaderBackend& operator=(DxcShaderBackend&&) = delete;

	bool IsValid() const noexcept { return m_compiler != nullptr && m_utils != nullptr; }

	static ShaderBackendCapabilities GetStaticCapabilities() noexcept;
	static std::uint64_t QueryBackendVersion();

	ShaderBackendCapabilities GetCapabilities() const override;

	std::string_view GetBackendName() const override;
	std::uint64_t GetBackendVersion() const override;

	CompiledShader Compile(const ShaderCompileRequest& request) override;

private:
	Microsoft::WRL::ComPtr<IDxcResult> CompileSource(const ShaderCompileRequest& request, const DxcBuffer& sourceBuffer, std::span<LPCWSTR> args, std::string& compilerOutput);
	static void BuildCompileArguments(
	    const ShaderCompileRequest& request,
	    const std::wstring& wSourcePath,
	    const std::wstring& wEntryPoint,
	    const std::wstring& wTargetProfile,
	    std::vector<std::wstring>& wDefines,
	    std::vector<LPCWSTR>& outArgs);

	static std::vector<std::uint8_t> ExtractBytecode(IDxcResult* result);

	static std::string ExtractErrorMessage(IDxcResult* result);
	static void ExtractSymbols(IDxcResult& result, ShaderDebugArtifactSet& artifacts);
	static std::string ExtractTextOutput(IDxcResult* result, DXC_OUT_KIND kind);
	static std::string ExtractPreprocessedSource(IDxcCompiler3& compiler, const DxcBuffer& sourceBuffer, const std::vector<LPCWSTR>& compileArgs);
	static std::string ExtractDisassembly(IDxcUtils& utils, IDxcCompiler3& compiler, std::span<const std::uint8_t> bytecode);
	ShaderDebugArtifactSet CaptureDebugArtifacts(
	    const ShaderCompileRequest& request,
	    IDxcResult& result,
	    const DxcBuffer& sourceBuffer,
	    std::span<const std::uint8_t> bytecode,
	    const std::vector<LPCWSTR>& compileArgs,
	    std::string_view compilerOutput) const;
	static std::vector<std::string> BuildDebugArgumentStrings(const std::vector<LPCWSTR>& compileArgs);
	static std::uint64_t QueryBackendVersion(IDxcCompiler3& compiler);

	Microsoft::WRL::ComPtr<IDxcCompiler3> m_compiler;
	Microsoft::WRL::ComPtr<IDxcUtils> m_utils;
	std::uint64_t m_backendVersion = 0;
};
