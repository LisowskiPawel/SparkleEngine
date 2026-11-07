#pragma once

#include "Compiler/ShaderCompileRequest.h"

#include <atomic>
#include <span>
#include <slang.h>

// Borrows the immutable compile request until the Slang session has been released.
class SlangSourceFiles final : public ISlangFileSystem
{
public:
	explicit SlangSourceFiles(std::span<const ShaderSourceFile> files) noexcept;

	SlangResult SLANG_MCALL queryInterface(const SlangUUID& uuid, void** object) noexcept override;
	std::uint32_t SLANG_MCALL addRef() noexcept override;
	std::uint32_t SLANG_MCALL release() noexcept override;
	void* SLANG_MCALL castAs(const SlangUUID& uuid) noexcept override;
	SlangResult SLANG_MCALL loadFile(const char* path, ISlangBlob** blob) noexcept override;

private:
	std::span<const ShaderSourceFile> m_files;
	std::atomic_uint32_t m_references = 1;
};
