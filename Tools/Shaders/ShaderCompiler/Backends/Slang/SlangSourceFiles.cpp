#include "PCH.h"

#include "Slang/SlangSourceFiles.h"

#include <algorithm>
#include <slang-com-helper.h>
#include <string_view>

SlangSourceFiles::SlangSourceFiles(std::span<const ShaderSourceFile> files) noexcept :
    m_files(files)
{
}

SlangResult SLANG_MCALL SlangSourceFiles::queryInterface(const SlangUUID& uuid, void** object) noexcept
{
	*object = castAs(uuid);
	if (*object == nullptr)
	{
		return SLANG_E_NO_INTERFACE;
	}
	addRef();
	return SLANG_OK;
}

std::uint32_t SLANG_MCALL SlangSourceFiles::addRef() noexcept
{
	return ++m_references;
}

std::uint32_t SLANG_MCALL SlangSourceFiles::release() noexcept
{
	const std::uint32_t remaining = --m_references;
	if (remaining == 0)
	{
		delete this;
	}
	return remaining;
}

void* SLANG_MCALL SlangSourceFiles::castAs(const SlangUUID& uuid) noexcept
{
	return uuid == ISlangUnknown::getTypeGuid() || uuid == ISlangCastable::getTypeGuid() || uuid == ISlangFileSystem::getTypeGuid() ? this : nullptr;
}

SlangResult SLANG_MCALL SlangSourceFiles::loadFile(const char* path, ISlangBlob** blob) noexcept
{
	*blob = nullptr;
	const auto found = std::ranges::find(m_files, std::string_view(path), &ShaderSourceFile::VirtualPath);
	if (found == m_files.end())
	{
		return SLANG_E_NOT_FOUND;
	}
	try
	{
		*blob = slang_createBlob(found->Contents.data(), found->Contents.size());
		return *blob != nullptr ? SLANG_OK : SLANG_FAIL;
	}
	catch (...)
	{
		return SLANG_FAIL;
	}
}
