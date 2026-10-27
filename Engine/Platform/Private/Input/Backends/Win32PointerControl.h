#pragma once

#include "Input/Mouse/MousePosition.h"

#include <optional>

// Reports whether a foreground target was available and capture was requested.
bool CaptureWin32ForegroundMouse();
void ReleaseWin32MouseCapture();
void HideWin32Cursor();
void ShowWin32Cursor();
std::optional<MousePosition> CenterWin32Cursor(void* windowHandle);
