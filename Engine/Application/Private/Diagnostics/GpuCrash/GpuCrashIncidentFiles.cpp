#include "PCH.h"

#include "Diagnostics/GpuCrash/GpuCrashIncidentSink.h"
#include "Diagnostics/GpuCrash/GpuCrashInputs.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Identifiers/Uuid.h"
#include "Core/Public/Json/JsonWriter.h"
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
			if (attributes == INVALID_FILE_ATTRIBUTES && GetLastError() != ERROR_FILE_NOT_FOUND && GetLastError() != ERROR_PATH_NOT_FOUND)
			{
				throw Diagnostics::Error("Cannot inspect GPU crash storage.");
			}
			if (attributes != INVALID_FILE_ATTRIBUTES && ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0 || (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0))
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
	if (m_files.size() + 2 >= MaximumFiles || payload.Bytes.size() > MaximumIncidentBytes - MetadataAllowance - m_writtenBytes)
	{
		MarkIncomplete();
		return;
	}

	m_writtenBytes += payload.Bytes.size();
	const auto path = m_directory / name;
	WriteDurableFile(path, payload.Bytes);
	Hash::Sha256Digest writtenHash;
	if (!Hash::TrySha256File(path, writtenHash, error) || Hash::Sha256ToHex(writtenHash) != hash)
	{
		throw Diagnostics::Error("GPU crash artifact did not match its written bytes.");
	}

	m_files.emplace(name, FileReceipt{std::move(hash), payload.Bytes.size()});
	m_hasDump |= payload.Id.Kind == RhiGpuCrashArtifactKind::Dump;
}

std::string GpuCrashIncidentSink::BuildManifest(std::string_view state) const
{
	std::int64_t nativeFailure;
	bool sdkFinished;
	{
		std::lock_guard lock(m_mutex);
		nativeFailure = m_nativeFailure;
		sdkFinished = m_sdkFinished;
	}

	Json::ObjectWriter manifest;
	manifest.WriteString("incidentId", m_identity);
	manifest.WriteString("state", state);
	manifest.WriteUInt64("childPid", GetCurrentProcessId());
	manifest.WriteRaw("candidate", m_inputs ? m_inputs->GetIdentity() : "null");
	manifest.WriteString("executableSha256", m_executableHash);
	manifest.WriteString("privacy", "private-native-artifacts");
	manifest.WriteString("sourceMapping", "unqualified");

	manifest.WriteRaw("nativeFailure", std::to_string(nativeFailure));
	manifest.WriteRaw("sdkFinished", sdkFinished ? "true" : "false");
	std::string files = "[";
	for (const auto& [name, receipt] : m_files)
	{
		Json::ObjectWriter file;
		file.WriteString("path", name);
		file.WriteString("sha256", receipt.Sha256);
		file.WriteUInt64("bytes", receipt.Bytes);
		if (files.size() > 1)
		{
			files += ',';
		}
		files += file.Finish();
	}
	files += ']';
	manifest.WriteRaw("files", files);
	return manifest.Finish();
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
	WriteDurableFile(temporary, std::as_bytes(std::span(json)));
	std::string error;
	if (!Files::TryFinalizeTemporaryFile(temporary, destination, error))
	{
		throw Diagnostics::Error(error);
	}

	if (state == "Ready")
	{
		std::lock_guard lock(m_mutex);
		m_readyPublished = m_nativeFailure == 0 && !m_finalizing;
	}
}

void GpuCrashIncidentSink::RemoveUnusedSlot() noexcept
{
	if (m_directory.empty() || m_keepSlot || m_hasDump || m_nativeFailure != 0)
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
