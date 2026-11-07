#include "GpuCrash/GpuCrashLaunch.h"

#include "SparkleLauncher/LevelRunOperations.h"
#include "SparkleLauncher/GpuCrashIncident.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Identifiers/Uuid.h"
#include "Core/Public/Json/JsonReader.h"
#include "Workspace/LauncherProductPaths.h"
#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "Core/Public/Strings/StringUtils.h"

#include <Windows.h>
#include <algorithm>
#include <array>
#include <system_error>

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

	std::string ReadGpuCrashState(const std::filesystem::path& manifestPath, std::string_view sessionId, std::uint32_t childPid, std::string* verifiedSnapshot)
	{
		const bool bInvalidIncidentIdentityfilename = !manifestPath.is_absolute() || manifestPath.filename() != "incident.json";
		const bool bInvalidIncidentIdentitynative = bInvalidIncidentIdentityfilename || (manifestPath.native().starts_with(L"\\\\"));
		const bool bIncidentIdentityIdentifiers = bInvalidIncidentIdentitynative || (!Identifiers::IsUuidV4String(sessionId));
		const bool bInvalidIncidentIdentity = bIncidentIdentityIdentifiers || (childPid == 0);
		
		if (bInvalidIncidentIdentity)
		{
			return {};
		}
		for (auto path = manifestPath; !path.empty(); path = path.parent_path())
		{
			const auto attributes = GetFileAttributesW(path.c_str());
			if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
			{
				return {};
			}
			if (path == path.root_path())
			{
				break;
			}
		}

		std::string json;
		std::string error;
		std::string identity;
		std::string state;
		std::uint32_t pid = 0;
		const bool bReceiptRead = Files::TryReadAllText(manifestPath, json, error, 32u * 1024u * 1024u);
		const bool bIdentityRead = bReceiptRead && Json::TryReadStringProperty(json, "incidentId", identity);
		const bool bIdentityMatches = bIdentityRead && identity == sessionId;
		const bool bPidRead = bIdentityMatches && Json::TryReadUnsignedIntegerProperty(json, "childPid", pid);
		const bool bPidMatches = bPidRead && pid == childPid;
		const bool bStateRead = bPidMatches && Json::TryReadStringProperty(json, "state", state);

		if (!bStateRead)
		{
			return {};
		}
		static constexpr std::array<std::string_view, 7> states{"Preparing", "Ready", "Collecting", "Collected", "Partial", "Timed out", "Unavailable"};
		if (std::find(states.begin(), states.end(), state) == states.end())
		{
			return {};
		}
		if (verifiedSnapshot != nullptr)
		{
			*verifiedSnapshot = std::move(json);
		}
		return state;
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
		std::string json;
		const auto state = ReadGpuCrashState(manifestPath, expected.SessionId, childPid, &json);
		if (state.empty() || state == "Preparing" || state == "Unavailable")
		{
			error = "Child has no current, native-ready GPU crash manifest for this launch; collection is unavailable or incomplete.";
			return Process::ChildReadinessDecision::Rejected;
		}
		Hash::Sha256Digest hash;
		std::string executableHash;
		const bool bExpectedHashRead = Json::TryReadStringProperty(json, "executableSha256", executableHash);
		const bool bExecutableHashed = bExpectedHashRead && Hash::TrySha256File(expected.ExecutablePath, hash, error);
		const bool bExecutableMatches = bExecutableHashed && executableHash == Hash::Sha256ToHex(hash);

		if (!bExecutableMatches)
		{
			error = "Child GPU crash identity does not match the selected executable.";
			return Process::ChildReadinessDecision::Rejected;
		}

		std::string_view candidate;
		std::string actualProfile;
		std::string actualApi;
		std::string actualExport;
		const bool bCandidateRead = Json::TryReadObjectProperty(json, "candidate", candidate);
		const bool bProfileRead = bCandidateRead && Json::TryReadStringProperty(candidate, "profile", actualProfile);
		const bool bProfileMatches = bProfileRead && actualProfile == expected.Profile;
		const bool bApiRead = bProfileMatches && Json::TryReadStringProperty(candidate, "graphicsApi", actualApi);
		const bool bApiMatches = bApiRead && Strings::EqualsIgnoreCase(actualApi, expected.GraphicsApi);
		const bool bExportHashRead = bApiMatches && Json::TryReadStringProperty(candidate, "shaderExportSha256", actualExport);
		const bool bExportHashed = bExportHashRead && Hash::TrySha256File(expected.ShaderExportPath, hash, error);
		const bool bExportMatches = bExportHashed && actualExport == Hash::Sha256ToHex(hash);

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
