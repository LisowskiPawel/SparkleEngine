#pragma once

struct InputBackendResult;
struct WindowMessageEvent;

InputBackendResult ProcessWin32InputMessage(const WindowMessageEvent& event);
