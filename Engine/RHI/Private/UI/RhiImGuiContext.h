#pragma once

struct ImGuiContext;

ImGuiContext* AcquireImGuiContext(ImGuiContext*& context, bool& bOwnsContext);
ImGuiContext* ActivateImGuiContext(ImGuiContext* context) noexcept;
void RestoreImGuiContext(ImGuiContext* context) noexcept;
void ReleaseImGuiContext(ImGuiContext*& context, bool& bOwnsContext) noexcept;
