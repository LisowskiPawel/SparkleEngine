#include "PCH.h"
#include "Core/Public/Hash/FileHash.h"

#include "Diagnostics/GpuCrash/GpuCrashIncidentSink.h"
#include "Diagnostics/GpuCrash/GpuCrashInputs.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Identifiers/Uuid.h"
#include <nlohmann/json.hpp>
#include "Core/Public/Paths/ProductUserStatePaths.h"

#include <Windows.h>
#include <format>
#include <string_view>

namespace GpuCrashIncidentFiles
{
	bool TryRemoveFile(const std::filesystem::path& path)
	{
		return DeleteFileW(path.c_str()) != 0 || GetLastError() == ERROR_FILE_NOT_FOUND;
	}

	void CheckDirectoryPath(const std::filesystem::path& directory)
	{
		if (!directory.is_absolute())
		{
			throw Diagnostics::Error("GPU crash storage requires an absolute product user-state path.");
		}
		std::filesystem::path current = directory.root_path();
		for (const auto& component : directory.relative_path())
		{
			current /= component;
			const DWORD attributes = GetFileAttributesW(current.c_str());
			if (attributes == INVALID_FILE_ATTRIBUTES)
			{
				const DWORD errorCode = GetLastError();
				const bool bMissing = errorCode == ERROR_FILE_NOT_FOUND || errorCode == ERROR_PATH_NOT_FOUND;
				if (!bMissing)
				{
					throw Diagnostics::Error("Cannot inspect GPU crash storage.");
				}
				continue;
			}

			const bool bReparsePoint = (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
			const bool bDirectory = (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			if (bReparsePoint || !bDirectory)
			{
				throw Diagnostics::Error("GPU crash storage cannot traverse a reparse point or a non-directory.");
			}
		}
	}
}

void GpuCrashIncidentSink::ReserveSlot()
{
	const auto root = Filesystem::GetProductUserStatePaths().CrashesRoot / "GpuCrash";
	GpuCrashIncidentFiles::CheckDirectoryPath(root);
	std::filesystem::create_directories(root);
	GpuCrashIncidentFiles::CheckDirectoryPath(root);
	if (!Identifiers::IsUuidV4String(m_identity))
	{
		throw Diagnostics::Error("GPU crash session UUID is invalid.");
	}
	for (unsigned index = 0; index < 4; ++index)
	{
		const std::string slotName = std::format("slot-{}.partial", index);
		const auto slot = root / slotName;
		if (CreateDirectoryW(slot.c_str(), nullptr))
		{
			m_directory = slot;
			const auto ownerBytes = std::as_bytes(std::span(m_identity));
			WriteDurableFile(m_directory / "owner.txt", ownerBytes);
			m_writtenBytes = ownerBytes.size();
			return;
		}
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			throw Diagnostics::Error("Cannot reserve private GPU crash storage.");
		}
	}
	throw Diagnostics::Error("All four GPU crash slots are occupied; remove an unwanted incident explicitly.");
}

void GpuCrashIncidentSink::WriteDurableFile(const std::filesystem::path& path, std::span<const std::byte> bytes)
{
	GpuCrashIncidentFiles::CheckDirectoryPath(path.parent_path());
	std::string error;
	if (!Files::TryWriteNewFileDurable(path, bytes, error))
	{
		throw Diagnostics::Error(error);
	}
}

std::string GpuCrashIncidentSink::BuildFileName(RhiGpuCrashArtifactId id)
{
	switch (id.Kind)
	{
		case RhiGpuCrashArtifactKind::Dump:
			return "crash.nv-gpudmp";
		case RhiGpuCrashArtifactKind::ShaderDebugInfo:
			return std::format("shader-{:016x}-{:016x}.nvdbg", id.Low, id.High);
		case RhiGpuCrashArtifactKind::ShaderIdentity:
			return std::format("shader-{:016x}-{:016x}.json", id.Low, id.High);
		case RhiGpuCrashArtifactKind::NativeDiagnostics:
			return std::format("native-{:016x}-{:016x}.json", id.Low, id.High);
	}
	throw Diagnostics::Error("Unknown GPU crash artifact kind.");
}

void GpuCrashIncidentSink::WritePayload(const Payload& payload)
{
	std::string hash;
	std::string error;
	if (!Hash::TrySha256Hex(payload.Bytes, hash, error))
	{
		throw Diagnostics::Error(error);
	}

	const std::string name = BuildFileName(payload.Id);
	if (const auto found = m_files.find(name); found != m_files.end())
	{
		if (found->second.Sha256 != hash || found->second.Bytes != payload.Bytes.size())
		{
			MarkIncomplete();
		}
		return;
	}
	const bool bFileLimitReached = m_files.size() + 2 >= MaximumFiles;
	const bool bByteLimitExceeded = payload.Bytes.size() > MaximumIncidentBytes - MetadataAllowance - m_writtenBytes;
	if (bFileLimitReached || bByteLimitExceeded)
	{
		MarkIncomplete();
		return;
	}

	m_writtenBytes += payload.Bytes.size();
	const auto path = m_directory / name;
	WriteDurableFile(path, payload.Bytes);
	Hash::Sha256Digest writtenHash;
	const bool bFileHashed = Hash::TrySha256File(path, writtenHash, error);
	const bool bHashMatches = bFileHashed && Hash::Sha256ToHex(writtenHash) == hash;
	if (!bHashMatches)
	{
		throw Diagnostics::Error("GPU crash artifact did not match its written bytes.");
	}

	m_files.emplace(name, FileReceipt{std::move(hash), payload.Bytes.size()});
	m_bHasDump |= payload.Id.Kind == RhiGpuCrashArtifactKind::Dump;
}

std::string GpuCrashIncidentSink::BuildManifest(std::string_view state) const
{
	std::int64_t nativeFailure;
	bool bSdkFinished;
	{
		std::lock_guard lock(m_mutex);
		nativeFailure = m_nativeFailure;
		bSdkFinished = m_bSdkFinished;
	}

	nlohmann::json manifest{
	    {"incidentId", m_identity},
	    {"state", state},
	    {"childPid", GetCurrentProcessId()},
	    {"candidate", m_inputs ? m_inputs->GetIdentity() : nlohmann::json(nullptr)},
	    {"executableSha256", m_executableHash},
	    {"privacy", "private-native-artifacts"},
	    {"sourceMapping", "unqualified"},
	    {"nativeFailure", nativeFailure},
	    {"sdkFinished", bSdkFinished},
	    {"files", nlohmann::json::array()}};

	for (const auto& [name, receipt] : m_files)
	{
		manifest["files"].push_back({{"path", name}, {"sha256", receipt.Sha256}, {"bytes", receipt.Bytes}});
	}
	return manifest.dump();
}

void GpuCrashIncidentSink::WriteManifest(std::string_view state)
{
	const std::string json = BuildManifest(state);
	if (json.size() > MetadataAllowance / 2)
	{
		throw Diagnostics::Error("GPU crash manifest exceeded its reserved storage.");
	}

	const auto destination = m_directory / "incident.json";
	const auto temporary = Files::BuildTemporaryPath(destination, ".pending");
	const auto manifestBytes = std::as_bytes(std::span(json));
	WriteDurableFile(temporary, manifestBytes);
	std::string error;
	if (!Files::TryFinalizeTemporaryFile(temporary, destination, error))
	{
		throw Diagnostics::Error(error);
	}

	if (state == "Ready")
	{
		std::lock_guard lock(m_mutex);
		m_bReadyPublished = m_nativeFailure == 0 && !m_bFinalizing;
	}
}

void GpuCrashIncidentSink::RemoveUnusedSlot() noexcept
{
	if (m_directory.empty() || m_bKeepSlot || m_bHasDump || m_nativeFailure != 0)
	{
		return;
	}

	try
	{
		GpuCrashIncidentFiles::CheckDirectoryPath(m_directory);
		std::string owner;
		std::string error;
		if (!Files::TryReadAllText(m_directory / "owner.txt", owner, error) || owner != m_identity)
		{
			return;
		}

		// Remove only the files this session published; preserve foreign entries.
		for (const auto& [name, receipt] : m_files)
		{
			(void) receipt;
			if (!GpuCrashIncidentFiles::TryRemoveFile(m_directory / name))
			{
				return;
			}
		}
		for (const char* name : {"incident.json.pending", "incident.json", "owner.txt"})
		{
			if (!GpuCrashIncidentFiles::TryRemoveFile(m_directory / name))
			{
				return;
			}
		}
		RemoveDirectoryW(m_directory.c_str());
	}
	catch (...)
	{
		// A cleanup failure retains the reserved slot for explicit user removal.
	}
}
