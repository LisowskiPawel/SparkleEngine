#pragma once

#include "Input/Mouse/MousePosition.h"

#include <optional>

bool CaptureWin32Mouse(void* windowHandle);
void ReleaseWin32MouseCapture(void* windowHandle);
void HideWin32Cursor();
void ShowWin32Cursor();
std::optional<MousePosition> CenterWin32Cursor(void* windowHandle);
