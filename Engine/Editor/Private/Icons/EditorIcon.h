#pragma once

#include <cstdint>
#include <string>

namespace EditorIcons
{
	enum class EditorIcon : std::uint8_t
	{
		None,
		Folder,
		FolderOpen,
		Camera,
		Light,
		DirectionalLight,
		PointLight,
		SpotLight,
		RectLight,
		Sky,
		StaticMesh,
		SkinnedMesh,
		EyeVisible,
		EyeHidden,
		Reset,
		Settings,
		Save,
		Shader,
		Reload,
		Search,
		Level,
		ViewMode,
		ViewLit,
		ViewDiffuse,
		ViewNormal,
		ViewRoughness,
		ViewMetallic,
		ViewEmissive,
		ViewAmbientOcclusion,
		ViewSubsurfaceColor,
		ViewSubsurfaceStrength,
		ViewDirectDiffuse,
		ViewDirectSpecular,
		ViewDirectSubsurface,
		Cpu,
		Gpu,
		Help,
		Clear,
		Copy,
		Console,



		Sort
	};
	const char* GetEditorIconGlyph(EditorIcon icon) noexcept;
	std::string MakeIconLabel(EditorIcon icon, const char* label);
}
