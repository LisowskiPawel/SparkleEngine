#include "PCH.h"
#include "GameApplication.h"

#include "GameFramework/Public/Rendering/RenderFrameSubmission.h"
#include "GameFramework/Public/Rendering/RenderViewCameraData.h"
#include "GameFramework/Public/Scene/Camera/CameraInputIntent.h"
#include "Input/CameraInputIntentCollector.h"
#include "Renderer.h"
#include "Time/Timer.h"
#include "World/GameWorld.h"

#include <utility>

void GameApplication::UpdateGame() noexcept
{
	const CameraInputIntent cameraIntent = m_cameraInputIntentCollector->Collect();
	UpdateGameFrame(&cameraIntent, nullptr);
}

CameraInputIntent GameApplication::CollectCameraInputIntent(float aspectRatio) noexcept
{
	return m_cameraInputIntentCollector != nullptr ? m_cameraInputIntentCollector->Collect(aspectRatio) : CameraInputIntent{};
}

void GameApplication::UpdateEditorGame(const RenderViewCameraData& renderCamera) noexcept
{
	UpdateGameFrame(nullptr, &renderCamera);
}

void GameApplication::UpdateGameFrame(const CameraInputIntent* worldCameraIntent, const RenderViewCameraData* renderCameraOverride) noexcept
{
	if (m_gameWorld && m_timer)
	{
		const std::uint64_t frameId = m_timer->GetFrameCount();
		m_renderer->BeginSimulationFrame(frameId);
		m_gameWorld->PublishCameraInputIntent(worldCameraIntent != nullptr ? *worldCameraIntent : CameraInputIntent{});
		const float deltaSeconds = static_cast<float>(m_timer->GetDelta(TimeDomain::Scaled, TimeUnit::Seconds));
		m_gameWorld->Update(deltaSeconds);
		m_renderer->EndSimulationFrame(frameId);
		SubmitRenderFrame(frameId, renderCameraOverride);
	}
}

void GameApplication::SubmitRenderFrame(std::uint64_t frameId, const RenderViewCameraData* renderCameraOverride)
{
	RenderFrameSubmission submission = m_gameWorld->ExtractRenderFrameSubmission(frameId);
	if (renderCameraOverride != nullptr)
	{
		submission.View.Camera = *renderCameraOverride;
	}
	m_renderer->SubmitRenderFrame(std::move(submission));
}

void GameApplication::SubmitViewportRenderRequest(ViewportRenderRequest request) noexcept
{
	if (m_renderer)
	{
		m_renderer->SubmitViewportRenderRequest(request);
	}
}
