#pragma once

#include "Input/InputSystem.h"
#include "Input/Dispatch/InputEventDispatcher.h"
#include "Input/Routing/InputFocusRouter.h"

struct InputSystem::DispatchState final
{
	InputEventDispatcher EventDispatcher;
	InputFocusRouter FocusRouter;
};
