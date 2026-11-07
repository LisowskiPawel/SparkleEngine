#include "PCH.h"
#include "UI/RhiImGuiContext.h"

#include <imgui.h>

ImGuiContext* AcquireImGuiContext(ImGuiContext*& context, bool& bOwnsContext)
{
	ImGuiContext* previousContext = ImGui::GetCurrentContext();
	context = previousContext;
	if (context == nullptr)
	{
		context = ImGui::CreateContext();
		bOwnsContext = true;
	}
	ImGui::SetCurrentContext(context);
	return previousContext;
}

ImGuiContext* ActivateImGuiContext(ImGuiContext* context) noexcept
{
	ImGuiContext* previousContext = ImGui::GetCurrentContext();
	ImGui::SetCurrentContext(context);
	return previousContext;
}

void RestoreImGuiContext(ImGuiContext* context) noexcept
{
	ImGui::SetCurrentContext(context);
}

void ReleaseImGuiContext(ImGuiContext*& context, bool& bOwnsContext) noexcept
{
	if (bOwnsContext)
	{
		ImGui::DestroyContext(context);
	}
	context = nullptr;
	bOwnsContext = false;
}
