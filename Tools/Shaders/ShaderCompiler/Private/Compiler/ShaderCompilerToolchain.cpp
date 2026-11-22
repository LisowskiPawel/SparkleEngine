#include "PCH.h"

#include "Compiler/ShaderCompilerToolchain.h"
#include "ShaderCompilerInputs.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Hash/FileHash.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Paths/FileSystemPaths.h"

#include <filesystem>
#include <format>
#include <utility>

static void RecordCompilerInput(ShaderCompilerToolchain& toolchain, const std::filesystem::path& path, std::string name)
{
	Hash::Sha256Digest digest;
	std::string error;
	if (!Hash::TrySha256File(path, digest, error))
	{
		throw Diagnostics::Error(std::format("Cannot identify shader compiler input '{}': {}", name, error));
	}
	std::string sha256 = Hash::Sha256ToHex(digest);
	toolchain.Version = Hash::ContinueFnv1a64(toolchain.Version, name.data(), name.size());
	toolchain.Version = Hash::ContinueFnv1a64(toolchain.Version, sha256.data(), sha256.size());
	toolchain.Files.push_back({.Name = std::move(name), .Sha256 = std::move(sha256)});
}

ShaderCompilerToolchain ReadShaderCompilerToolchain()
{
	ShaderCompilerToolchain toolchain{.Version = Hash::kFnv64OffsetBasis};
	const std::filesystem::path executable = Filesystem::GetExecutablePath();
	RecordCompilerInput(toolchain, executable, executable.filename().generic_string());
	const std::filesystem::path directory = executable.parent_path();
	for (const std::string_view name : kShaderCompilerInputFiles)
	{
		RecordCompilerInput(toolchain, directory / std::filesystem::path(name), std::string(name));
	}
	toolchain.Version = Hash::FinalizeFnv1a64(toolchain.Version);
	return toolchain;
}
