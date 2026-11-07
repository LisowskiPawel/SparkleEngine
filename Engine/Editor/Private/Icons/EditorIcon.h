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
		Material,
		EyeVisible,
		EyeHidden,
		Reset,
		Filter,
		Settings,
		Save,
		Shader,
		Refresh,
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
		SourceFile,
		Reflection,
		Disassembly,
		CompileRequest,
		Sort
	};

	const char* GetEditorIconGlyph(EditorIcon icon) noexcept;
	std::string MakeIconLabel(EditorIcon icon, const char* label);
}
