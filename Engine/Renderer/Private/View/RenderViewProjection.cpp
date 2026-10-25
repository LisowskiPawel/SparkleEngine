#include "PCH.h"
#include "View/RenderViewProjection.h"

#include "Config/DepthConvention.h"
#include "Core/Public/Math/WorldCoordinateSystem.h"
#include "View/RenderView.h"

#include <algorithm>

static RhiViewport BuildViewport(RenderViewportExtent extent) noexcept
{
	return RhiViewport{.X = 0.0f, .Y = 0.0f, .Width = static_cast<float>(extent.Width), .Height = static_cast<float>(extent.Height), .MinDepth = 0.0f, .MaxDepth = 1.0f};
}

static RhiRect BuildScissorRect(RenderViewportExtent extent) noexcept
{
	return RhiRect{.Left = 0, .Top = 0, .Right = static_cast<std::int32_t>(extent.Width), .Bottom = static_cast<std::int32_t>(extent.Height)};
}

void BuildRenderViewCamera(RenderView& view, RenderViewportExtent projectionExtent) noexcept
{
	const float projectionWidth = static_cast<float>((std::max) (projectionExtent.Width, 1u));
	const float projectionHeight = static_cast<float>((std::max) (projectionExtent.Height, 1u));
	view.camera.AspectRatio = projectionWidth / projectionHeight;

	const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&view.camera.Position);
	const DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&view.camera.Direction);
	const DirectX::XMVECTOR target = DirectX::XMVectorAdd(position, direction);
	const DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(WorldCoordinates::kUpX, WorldCoordinates::kUpY, WorldCoordinates::kUpZ, 0.0f);
	const DirectX::XMMATRIX worldToView = DirectX::XMMatrixLookAtLH(position, target, worldUp);

	const float nearZ = view.camera.NearZ;
	const float farZ = view.camera.FarZ;
	DirectX::XMMATRIX viewToClip;
	if (view.camera.ProjectionKind == CameraProjectionKind::Orthographic)
	{
		const float height = (std::max) (view.camera.OrthographicHeightMeters, 0.001f);
		const float width = height * (std::max) (view.camera.AspectRatio, 0.001f);
		viewToClip = DepthConvention::CreateOrthographicOffCenterLH(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, nearZ, farZ);
	}
	else
	{
		viewToClip = DepthConvention::CreatePerspectiveFovLH(DirectX::XMConvertToRadians(view.camera.FovYDegrees), view.camera.AspectRatio, nearZ, farZ);
	}

	const DirectX::XMMATRIX worldToClip = DirectX::XMMatrixMultiply(worldToView, viewToClip);
	DirectX::XMStoreFloat4x4(&view.cameraUniform.ViewMTX, worldToView);
	DirectX::XMStoreFloat4x4(&view.cameraUniform.ProjectionMTX, viewToClip);
	DirectX::XMStoreFloat4x4(&view.cameraUniform.ViewProjMTX, worldToClip);
	DirectX::XMStoreFloat4x4(&view.cameraUniform.InvViewMTX, DirectX::XMMatrixInverse(nullptr, worldToView));
	DirectX::XMStoreFloat4x4(&view.cameraUniform.InvProjectionMTX, DirectX::XMMatrixInverse(nullptr, viewToClip));
	view.cameraUniform.Position = view.camera.Position;
	view.cameraUniform.NearZ = nearZ;
	view.cameraUniform.FarZ = farZ;
	view.cameraUniform.Direction = view.camera.Direction;
	view.frustum.ExtractFromViewProjection(view.cameraUniform.ViewProjMTX);
}

void BuildRenderViewRasterState(RenderView& view, RenderViewportExtent extent) noexcept
{
	view.viewport = BuildViewport(extent);
	view.scissorRect = BuildScissorRect(extent);
	const float renderWidth = static_cast<float>((std::max) (extent.Width, 1u));
	const float renderHeight = static_cast<float>((std::max) (extent.Height, 1u));
	view.uniform.ViewportSize = {renderWidth, renderHeight};
	view.uniform.ViewportSizeInv = {1.0f / renderWidth, 1.0f / renderHeight};
	view.uniform.RenderViewModeIndex = static_cast<std::uint32_t>(view.viewMode);
}
