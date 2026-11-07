#pragma once

#include "GameFramework/Public/Rendering/RenderViewCameraData.h"

struct RenderViewInput final
{
	RenderViewCameraData Camera;
	bool bCameraCut = false;
	bool bCameraTeleported = false;
};
