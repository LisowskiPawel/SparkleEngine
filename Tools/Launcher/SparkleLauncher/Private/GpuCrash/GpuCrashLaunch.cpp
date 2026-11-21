#include "GpuCrash/GpuCrashLaunch.h"
#include "Core/Public/Hash/FileHash.h"

#include "SparkleLauncher/LevelRunOperations.h"
#include "SparkleLauncher/GpuCrashIncident.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Identifiers/Uuid.h"
#include "Core/Public/Json/JsonInputAdmission.h"
#include "Core/Public/Diagnostics/Error.h"

#include <nlohmann/json.hpp>
#include "Workspace/LauncherProductPaths.h"
#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "Core/Public/Strings/StringUtils.h"

#include <Windows.h>
#include <algorithm>
#include <array>
#include <system_error>
#include <limits>

namespace SparkleLauncher
{
	bool PrepareGpuCrashLaunch(LevelRunOperationPlan& plan)
	{
		if (plan.Request.GpuCrashMode == RhiGpuCrashMode::Off)
		{
			if (!plan.Request.ShaderExportPath.empty())
			{
				plan.ReadinessMessages.emplace_back("Select Aftermath to use a shader export.");
				return false;
			}
			return true;
		}
		const bool bAftermathRequested = plan.Request.GpuCrashMode == RhiGpuCrashMode::Aftermath;
		const bool bSourceBuildAvailable = bAftermathRequested && HasBuildSources(plan.Request.WorkspaceRoot);
		const bool bCaptureCompatible = bSourceBuildAvailable && plan.Request.CaptureProvider == ExternalCaptureProvider::None;
		const bool bGpuCrashLaunchSupported = bCaptureCompatible && !plan.Request.ProductProfile.starts_with("Debug");
		if (!bGpuCrashLaunchSupported)
		{
			plan.ReadinessMessages.emplace_back("Aftermath requires a Development/Shipping diagnostic product and a separate launch from frame capture.");
			return false;
		}
		if (!plan.Request.ShaderExportPath.is_absolute() || plan.Request.ShaderExportPath.filename() != "shader-export.json")
		{
			plan.ReadinessMessages.emplace_back("Provide the exact absolute shader-export.json path returned by the diagnostic shader cook.");
			return false;
		}

		std::error_code error;
		if (!std::filesystem::is_regular_file(plan.Request.ShaderExportPath, error) || error)
		{
			plan.ReadinessMessages.emplace_back("The exact requested shader export is missing or unreadable; complete the diagnostic cook first.");
			return false;
		}
		plan.SessionId = Identifiers::CreateUuidV4String();
		plan.ReadinessMessages.emplace_back("Aftermath inputs will be qualified by the child before devices; native collection is not active yet.");
		const auto state = Filesystem::ResolveDevelopmentProductUserStatePaths(plan.Request.WorkspaceRoot, plan.Request.ContentId);
		const auto incidentRoot = state.CrashesRoot / "GpuCrash";
		plan.PlannedEffects.emplace_back("Reserve one private incident slot beneath " + Strings::ToNarrow(incidentRoot.wstring()) + ".");
		return true;
	}

	std::filesystem::path ReadGpuCrashAnnouncement(std::string_view output)
	{
		return ReadProcessOutputPath(output, "SPARKLE_GPU_CRASH_INCIDENT ");
	}

	static bool IsLocalGpuCrashManifest(const std::filesystem::path& manifestPath)
	{
		const bool bInvalidLocation = !manifestPath.is_absolute() || manifestPath.filename() != "incident.json";
		if (bInvalidLocation || manifestPath.native().starts_with(L"\\\\"))
		{
			return false;
		}
		for (auto path = manifestPath; !path.empty(); path = path.parent_path())
		{
			const auto attributes = GetFileAttributesW(path.c_str());
			if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
			{
				return false;
			}
			if (path == path.root_path())
			{
				break;
			}
		}

		return true;
	}

	static nlohmann::json ParseGpuCrashManifest(std::string_view text)
	{
		Json::InputAdmission admission(text, 32u * 1024u * 1024u);
		bool bCandidate = false;
		bool bFiles = false;
		std::size_t fileCount = 0;

		auto document = nlohmann::json::parse(
		    text.begin(),
		    text.end(),
		    [&](int depth, nlohmann::json::parse_event_t event, nlohmann::json& value)
		    {
			    using Event = nlohmann::json::parse_event_t;
			    admission.ReadEvent(depth, event);
			    const bool bContainerStart = event == Event::object_start || event == Event::array_start;
			    const bool bEntryStart = bContainerStart || event == Event::value;
			    const bool bFileDepth = bFiles && depth == 2;
			    if (bFileDepth && bEntryStart && ++fileCount > 65'535)
			    {
				    throw Diagnostics::Error("GPU crash incident exceeds its file limit.");
			    }
			    if (event == Event::key)
			    {
				    const auto& key = value.get_ref<const std::string&>();
				    const std::array<std::string_view, 8> rootFields{"incidentId", "childPid", "state", "executableSha256", "candidate", "files", "nativeFailure", "sdkFinished"};
				    const std::array<std::string_view, 4> candidateFields{"profile", "graphicsApi", "shaderExportSha256", "cpuPdbAge"};
				    const bool bRootField = depth == 1 && std::find(rootFields.begin(), rootFields.end(), key) != rootFields.end();
				    const bool bCandidateDepth = bCandidate && depth == 2;
				    const bool bCandidateField = bCandidateDepth && std::find(candidateFields.begin(), candidateFields.end(), key) != candidateFields.end();
				    const std::array<std::string_view, 3> fileFields{"path", "sha256", "bytes"};
				    const bool bFileMemberDepth = bFiles && depth == 3;
				    const bool bFileField = bFileMemberDepth && std::find(fileFields.begin(), fileFields.end(), key) != fileFields.end();
				    if (depth == 1)
				    {
					    bCandidate = key == "candidate";
					    bFiles = key == "files";
					    fileCount = 0;
				    }
				    const bool bIdentityField = bRootField || bCandidateField;
				    const bool bConsumed = bIdentityField || bFileField;
				    return bConsumed && admission.ReadKey(depth, key);
			    }
			    return true;
		    });

		return document;
	}

	static bool HasValidGpuCrashCollection(const nlohmann::json& document, std::size_t manifestBytes)
	{
		const auto& files = document.at("files");
		const auto& failure = document.at("nativeFailure");
		const bool bFailureInteger = failure.is_number_integer();
		const bool bFailureOverflow = failure.is_number_unsigned() && failure.get<std::uint64_t>() > (std::numeric_limits<std::int64_t>::max)();
		const bool bInvalidFailure = !bFailureInteger || bFailureOverflow;
		const bool bInvalidFiles = !files.is_array() || files.size() > 65'535;
		if (bInvalidFailure || bInvalidFiles || !document.at("sdkFinished").is_boolean())
		{
			return false;
		}
		std::uint64_t totalBytes = manifestBytes;
		for (const auto& file : files)
		{
			const bool bPathString = file.at("path").is_string();
			const bool bHashString = file.at("sha256").is_string();
			const bool bIdentityStrings = bPathString && bHashString;
			if (!bIdentityStrings || !file.at("bytes").is_number_unsigned())
			{
				return false;
			}
			const auto bytes = file.at("bytes").get<std::uint64_t>();
			if (bytes > 1024u * 1024u * 1024u - totalBytes)
			{
				return false;
			}
			totalBytes += bytes;
		}
		const auto candidate = document.find("candidate");
		if (candidate != document.end() && !candidate->is_null())
		{
			if (!candidate->is_object())
			{
				return false;
			}
			const auto age = candidate->find("cpuPdbAge");
			if (age != candidate->end() && !age->is_number_unsigned())
			{
				return false;
			}
		}
		return true;
	}

	static bool MatchesGpuCrashSession(const nlohmann::json& document, std::string_view sessionId, std::uint32_t childPid)
	{
		const auto& pid = document.at("childPid");
		const bool bIdentityMatches = document.at("incidentId").get_ref<const std::string&>() == sessionId;
		const bool bPidType = bIdentityMatches && pid.is_number_unsigned();
		const bool bPidMatches = bPidType && pid.get<std::uint64_t>() == childPid;
		if (!bPidMatches)
		{
			return false;
		}
		const auto& state = document.at("state").get_ref<const std::string&>();
		static constexpr std::array<std::string_view, 7> states{"Preparing", "Ready", "Collecting", "Collected", "Partial", "Timed out", "Unavailable"};
		if (std::find(states.begin(), states.end(), state) == states.end())
		{
			return false;
		}
		return true;
	}

	static nlohmann::json ReadGpuCrashSnapshot(const std::filesystem::path& manifestPath, std::string_view sessionId, std::uint32_t childPid)
	{
		const bool bValidSession = Identifiers::IsUuidV4String(sessionId) && childPid != 0;
		if (!bValidSession || !IsLocalGpuCrashManifest(manifestPath))
		{
			return {};
		}

		std::string text;
		std::string error;
		if (!Files::TryReadAllText(manifestPath, text, error, 32u * 1024u * 1024u))
		{
			return {};
		}

		try
		{
			auto document = ParseGpuCrashManifest(text);
			const bool bValidCollection = HasValidGpuCrashCollection(document, text.size());
			const bool bSessionMatches = bValidCollection && MatchesGpuCrashSession(document, sessionId, childPid);
			if (!bSessionMatches)
			{
				return {};
			}
			return document;
		}
		catch (const std::exception&)
		{
			return {};
		}
	}

	std::string ReadGpuCrashState(const std::filesystem::path& manifestPath, std::string_view sessionId, std::uint32_t childPid)
	{
		const auto document = ReadGpuCrashSnapshot(manifestPath, sessionId, childPid);
		return document.is_null() ? std::string{} : document.at("state").get<std::string>();
	}

	std::string ReadGpuCrashCollectionState(const GpuCrashIncident& incident)
	{
		try
		{
			auto state = ReadGpuCrashState(incident.ManifestPath, incident.SessionId, incident.ChildPid);
			if (state.empty())
			{
				return "Unavailable or stale manifest";
			}
			if (state == "Ready" || state == "Preparing" || state == "Collecting")
			{
				const HANDLE child = OpenProcess(SYNCHRONIZE, FALSE, incident.ChildPid);
				if (child == nullptr)
				{
					return "Incomplete or process unavailable; last state " + state;
				}

				const DWORD observation = WaitForSingleObject(child, 0);
				CloseHandle(child);
				if (observation == WAIT_OBJECT_0)
				{
					return "Incomplete; child exited after " + state;
				}
				if (observation != WAIT_TIMEOUT)
				{
					return "Process observation unavailable; last state " + state;
				}
			}
			return state;
		}
		catch (...)
		{
			return "Incident observation unavailable";
		}
	}

	Process::ChildReadinessDecision VerifyGpuCrashReadiness(const GpuCrashReadinessExpectation& expected, std::uint32_t childPid, std::string_view output, std::string& error)
	{
		const auto manifestPath = ReadGpuCrashAnnouncement(output);
		const auto document = ReadGpuCrashSnapshot(manifestPath, expected.SessionId, childPid);
		const auto state = document.is_null() ? std::string{} : document.at("state").get<std::string>();
		if (state.empty() || state == "Preparing" || state == "Unavailable")
		{
			error = "Child has no current, native-ready GPU crash manifest for this launch; collection is unavailable or incomplete.";
			return Process::ChildReadinessDecision::Rejected;
		}
		Hash::Sha256Digest hash;
		const auto executableHash = document.find("executableSha256");
		const bool bExpectedHashRead = executableHash != document.end() && executableHash->is_string();
		const bool bExecutableHashed = bExpectedHashRead && Hash::TrySha256File(expected.ExecutablePath, hash, error);
		const bool bExecutableMatches = bExecutableHashed && executableHash->get_ref<const std::string&>() == Hash::Sha256ToHex(hash);

		if (!bExecutableMatches)
		{
			error = "Child GPU crash identity does not match the selected executable.";
			return Process::ChildReadinessDecision::Rejected;
		}

		bool bExportMatches = false;
		try
		{
			const auto& candidate = document.at("candidate");
			const auto& actualProfile = candidate.at("profile").get_ref<const std::string&>();
			const auto& actualApi = candidate.at("graphicsApi").get_ref<const std::string&>();
			const auto& actualExport = candidate.at("shaderExportSha256").get_ref<const std::string&>();
			const bool bProfileMatches = actualProfile == expected.Profile;
			const bool bApiMatches = bProfileMatches && Strings::EqualsIgnoreCase(actualApi, expected.GraphicsApi);
			const bool bExportHashed = bApiMatches && Hash::TrySha256File(expected.ShaderExportPath, hash, error);
			bExportMatches = bExportHashed && actualExport == Hash::Sha256ToHex(hash);
		}
		catch (const nlohmann::json::exception&)
		{
			// Missing or wrong-typed candidate fields reject this handoff.
		}

		if (!bExportMatches)
		{
			error = "Child session belongs to another profile/API or shader export.";
			return Process::ChildReadinessDecision::Rejected;
		}
		if (state != "Ready")
		{
			error = "Child entered GPU failure collection before handoff; incident retained and native finalization left to the child.";
			return Process::ChildReadinessDecision::ReleaseWithoutReadiness;
		}
		return Process::ChildReadinessDecision::Ready;
	}
}
