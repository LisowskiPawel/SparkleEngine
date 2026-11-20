#include "PCH.h"
#include "Editor/Public/Icons/EditorIconService.h"

#include "Editor/Public/Icons/EditorIconAsset.h"

#include <imgui.h>

#include <algorithm>
#include <cstring>
#include <vector>

class EditorIconService::Implementation final
{
public:
	Implementation() :
	    m_atlas(ImGui::GetIO().Fonts)
	{
	}

	~Implementation() noexcept
	{
		for (const RegisteredIcon& icon : m_icons)
		{
			m_atlas->RemoveCustomRect(icon.Rectangle);
		}
	}

	bool DrawButton(const EditorIconAsset& asset, const char* id, float imageSize) noexcept;

private:
	struct RegisteredIcon final
	{
		const EditorIconAsset* Asset;
		ImFontAtlasRectId Rectangle;
	};

	ImFontAtlasRect Resolve(const EditorIconAsset& asset) noexcept;

	ImFontAtlas* const m_atlas;
	std::vector<RegisteredIcon> m_icons;
};

ImFontAtlasRect EditorIconService::Implementation::Resolve(const EditorIconAsset& asset) noexcept
{
	auto found = std::ranges::find(m_icons, &asset, &RegisteredIcon::Asset);
	ImFontAtlasRect rectangle;
	if (found != m_icons.end() && m_atlas->GetCustomRect(found->Rectangle, &rectangle))
	{
		return rectangle;
	}

	IM_ASSERT(asset.Extent >= 1 && asset.Extent <= 512);
	IM_ASSERT(asset.Pixels.size() == static_cast<std::size_t>(asset.Extent) * asset.Extent * 4);
	const ImFontAtlasRectId rectangleId = m_atlas->AddCustomRect(asset.Extent, asset.Extent, &rectangle);
	IM_ASSERT(rectangleId != ImFontAtlasRectId_Invalid && m_atlas->TexData->Format == ImTextureFormat_RGBA32);
	m_atlas->TexPixelsUseColors = true;
	const std::size_t rowByteSize = static_cast<std::size_t>(asset.Extent) * 4;
	for (int row = 0; row < asset.Extent; ++row)
	{
		const auto* source = asset.Pixels.data() + static_cast<std::size_t>(row) * rowByteSize;
		void* destination = m_atlas->TexData->GetPixelsAt(rectangle.x, rectangle.y + row);
		std::memcpy(destination, source, rowByteSize);
	}
	if (found != m_icons.end())
	{
		found->Rectangle = rectangleId;
	}
	else
	{
		m_icons.push_back({&asset, rectangleId});
	}
	// AddCustomRect queues the upload; the existing render packet transfers pixels.
	return rectangle;
}

bool EditorIconService::Implementation::DrawButton(const EditorIconAsset& asset, const char* id, float imageSize) noexcept
{
	// Resolve every draw: packing/growth can change both coordinates and TexRef.
	const ImFontAtlasRect rectangle = Resolve(asset);
	if (imageSize <= 0.0f)
	{
		imageSize = ImGui::GetFontSize();
	}
	const float padding = ImGui::GetStyle().FramePadding.y;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding, padding));
	const bool bPressed = ImGui::ImageButton(id, m_atlas->TexRef, ImVec2(imageSize, imageSize), rectangle.uv0, rectangle.uv1);
	ImGui::PopStyleVar();
	return bPressed;
}

EditorIconService::EditorIconService() :
    m_implementation(std::make_unique<Implementation>())
{
}

EditorIconService::~EditorIconService() noexcept = default;

bool EditorIconService::DrawButton(const EditorIconAsset& asset, const char* id, float imageSize) noexcept
{
	return m_implementation->DrawButton(asset, id, imageSize);
}
