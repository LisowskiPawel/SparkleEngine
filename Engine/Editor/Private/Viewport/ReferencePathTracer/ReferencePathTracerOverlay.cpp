#include "PCH.h"

#include "Viewport/ReferencePathTracer/ReferencePathTracerOverlay.h"

#include "Viewport/ViewportOutputAction.h"
#include "Renderer/Public/Viewport/ViewportContracts.h"

#include <algorithm>
#include <array>
#include <cstdio>

#include <imgui.h>

constexpr std::array ProgressStateLabels = {"Inactive", "Resetting", "Accumulating", "Paused", "Unavailable", "Complete"};

constexpr std::array ProgressReasonLabels = {
    "None",
    "View changed",
    "Camera changed",
    "Scene geometry changed",
    "Deformation changed",
    "Material changed",
    "Lighting changed",
    "Environment changed",
    "Shader changed",
    "Execution route changed",
    "Backend changed",
    "Manual restart",
    "Resumed retained prefix",
    "Retained prefix released by memory policy",
    "Perspective camera and a valid viewport are required",
    "The scene contains unsupported material transport",
    "Required GPU ray tracing or accumulation support is unavailable",
    "Another viewport owns the reference accumulator"};

static const char* GetStateLabel(ViewportRenderProgressState state) noexcept
{
	const std::size_t index = static_cast<std::size_t>(state);
	return index < ProgressStateLabels.size() ? ProgressStateLabels[index] : "Unknown";
}

static const char* GetReasonLabel(ViewportRenderProgressReason reason) noexcept
{
	const std::size_t index = static_cast<std::size_t>(reason);
	return index < ProgressReasonLabels.size() ? ProgressReasonLabels[index] : "Unknown reason";
}

static void BeginReferenceOverlay(const ViewportRenderProgress& progress) noexcept
{
	const bool bUnavailable = progress.State == ViewportRenderProgressState::Unavailable;
	const bool bComplete = progress.State == ViewportRenderProgressState::Complete;
	const float overlayHeight = bUnavailable ? 118.0f : (bComplete ? 92.0f : 142.0f);

	ImGui::BeginChild(
	    "##ViewportRenderProgress",
	    ImVec2(330.0f, overlayHeight),
	    ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY,
	    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImGui::TextUnformatted("Reference Path Tracer");
}

static void DrawUnavailableSession(const ViewportRenderProgress& progress, ReferencePathTracerOverlayActions& actions) noexcept
{
	const char* reasonLabel = GetReasonLabel(progress.Reason);
	ImGui::TextUnformatted("Unavailable");
	ImGui::TextWrapped("%s", reasonLabel);
	if (progress.Reason == ViewportRenderProgressReason::SessionCapacity)
	{
		ImGui::Text("Owned by viewport %llu", static_cast<unsigned long long>(progress.OwnerViewportId));
		if (ImGui::Button("Transfer"))
		{
			actions.RenderAction = ViewportRenderAction::Transfer;
		}
	}
	else if (ImGui::Button("Use Lit"))
	{
		actions.ViewMode = RenderViewMode::Lit;
	}
}

static void DrawAccumulationProgress(const ViewportRenderProgress& progress) noexcept
{
	const bool bComplete = progress.State == ViewportRenderProgressState::Complete;
	const char* stateLabel = GetStateLabel(progress.State);
	const char* reasonLabel = GetReasonLabel(progress.Reason);
	const float fraction = progress.TargetSamples == 0 ? 0.0f : (std::min) (1.0f, static_cast<float>(progress.CompletedSamples) / static_cast<float>(progress.TargetSamples));
	char sampleLabel[64] = {};
	std::snprintf(sampleLabel, sizeof(sampleLabel), "%llu / %llu SPP", static_cast<unsigned long long>(progress.CompletedSamples), static_cast<unsigned long long>(progress.TargetSamples));
	ImGui::Text("%s%s", stateLabel, bComplete ? " - target prefix reached" : "");
	ImGui::ProgressBar(fraction, ImVec2(-1.0f, 0.0f), sampleLabel);

	if (progress.SamplesPerSecond > 0.0 && !bComplete)
	{
		ImGui::Text("%.2f Msamples/s  ETA about %.0f s", progress.SamplesPerSecond / 1000000.0, progress.EstimatedSecondsRemaining);
	}
	else if (!bComplete)
	{
		ImGui::TextUnformatted("Throughput and ETA calculating");
	}
	if (progress.Reason != ViewportRenderProgressReason::None)
	{
		ImGui::TextWrapped("Last event: %s", reasonLabel);
	}
}

static void DrawPlaybackControls(const ViewportRenderProgress& progress, ReferencePathTracerOverlayActions& actions) noexcept
{
	const bool bPaused = progress.State == ViewportRenderProgressState::Paused;
	const bool bComplete = progress.State == ViewportRenderProgressState::Complete;

	if (bPaused)
	{
		if (ImGui::Button("Resume"))
		{
			actions.RenderAction = ViewportRenderAction::Resume;
		}
	}
	else if (!bComplete)
	{
		ImGui::BeginDisabled(!progress.bRetentionAvailable);
		if (ImGui::Button("Pause"))
		{
			actions.RenderAction = ViewportRenderAction::Pause;
		}
		ImGui::EndDisabled();
		if (!progress.bRetentionAvailable && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("Retention unavailable for the current GPU memory budget");
		}
	}

	if (!bComplete || bPaused)
	{
		ImGui::SameLine();
	}
	if (ImGui::Button("Restart"))
	{
		actions.RenderAction = ViewportRenderAction::Restart;
	}
}

static void DrawOutputActions(const ViewportRenderProgress& progress, ReferencePathTracerOverlayActions& actions) noexcept
{
	const bool bUnavailable = progress.State == ViewportRenderProgressState::Unavailable;
	const bool bComplete = progress.State == ViewportRenderProgressState::Complete;
	const bool bHasPrefix = !bUnavailable && progress.CompletedSamples > 0;
	ImGui::BeginDisabled(!bHasPrefix || bComplete);
	if (ImGui::Button("Save partial prefix"))
	{
		actions.RenderAction = ViewportRenderAction::Pause;
		actions.OutputAction = ViewportOutputAction::SaveCurrentPrefix;
	}
	ImGui::EndDisabled();
	ImGui::SameLine();
	ImGui::BeginDisabled(!bComplete);
	if (ImGui::Button("Save complete"))
	{
		actions.OutputAction = ViewportOutputAction::SaveComplete;
	}
	ImGui::EndDisabled();
	ImGui::BeginDisabled(!bHasPrefix);
	if (ImGui::Button("Save checkpoint"))
	{
		actions.RenderAction = ViewportRenderAction::Pause;
		actions.OutputAction = ViewportOutputAction::SaveCheckpoint;
	}
	ImGui::EndDisabled();
	ImGui::TextDisabled("Scene-linear Reference radiance at the session extent; final viewport color is not exported.");
}

static void DrawDetails(const ViewportRenderProgress& progress, ReferencePathTracerOverlayActions& actions) noexcept
{
	const bool bUnavailable = progress.State == ViewportRenderProgressState::Unavailable;
	if (!bUnavailable || progress.Reason == ViewportRenderProgressReason::SessionCapacity)
	{
		ImGui::SameLine();
	}
	if (ImGui::Button("Details"))
	{
		ImGui::OpenPopup("Reference Path Tracer Details");
	}
	if (!ImGui::BeginPopup("Reference Path Tracer Details"))
	{
		return;
	}
	const char* stateLabel = GetStateLabel(progress.State);
	const char* reasonLabel = GetReasonLabel(progress.Reason);
	ImGui::Text("State: %s", stateLabel);
	ImGui::Text("Committed prefix: %llu / %llu SPP", static_cast<unsigned long long>(progress.CompletedSamples), static_cast<unsigned long long>(progress.TargetSamples));
	ImGui::Text("Last event: %s", reasonLabel);
	ImGui::Text("Discarded prefix: %llu SPP", static_cast<unsigned long long>(progress.DiscardedSamples));
	ImGui::Separator();
	ImGui::TextUnformatted("Reference radiance: scene-linear HDR sample mean");
	ImGui::TextUnformatted("Final color: viewport presentation derived from Reference radiance");
	ImGui::SeparatorText("Evidence / Output");
	DrawOutputActions(progress, actions);
	ImGui::EndPopup();
}

ReferencePathTracerOverlayActions DrawReferencePathTracerOverlay(const ViewportRenderProgress& progress) noexcept
{
	ReferencePathTracerOverlayActions actions;
	BeginReferenceOverlay(progress);
	if (progress.State == ViewportRenderProgressState::Unavailable)
	{
		DrawUnavailableSession(progress, actions);
	}
	else
	{
		DrawAccumulationProgress(progress);
		DrawPlaybackControls(progress, actions);
	}
	DrawDetails(progress, actions);
	ImGui::EndChild();
	return actions;
}
