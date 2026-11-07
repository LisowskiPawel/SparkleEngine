#include "PCH.h"
#include "Icons/EditorIcon.h"

namespace EditorIcons
{
	// Semantic icons map directly to the Font Awesome Free Solid 6.7.1 font asset.
	const char* GetEditorIconGlyph(EditorIcon icon) noexcept
	{
		switch (icon)
		{
			case EditorIcon::Folder:
				return "\xef\x81\xbb";
			case EditorIcon::FolderOpen:
				return "\xef\x81\xbc";
			case EditorIcon::Camera:
				return "\xef\x80\xb0";
			case EditorIcon::Light:
				return "\xef\x83\xa7";
			case EditorIcon::DirectionalLight:
				return "\xef\x86\x85";
			case EditorIcon::PointLight:
				return "\xef\x86\x92";
			case EditorIcon::SpotLight:
				return "\xef\x85\x80";
			case EditorIcon::RectLight:
				return "\xef\x83\x88";
			case EditorIcon::Sky:
				return "\xef\x83\x82";
			case EditorIcon::StaticMesh:
				return "\xef\x86\xb2";
			case EditorIcon::SkinnedMesh:
				return "\xef\x86\x83";
			case EditorIcon::Material:
				return "\xef\x94\xbf";
			case EditorIcon::EyeVisible:
				return "\xef\x81\xae";
			case EditorIcon::EyeHidden:
				return "\xef\x81\xb0";
			case EditorIcon::Reset:
				return "\xef\x83\xa2";
			case EditorIcon::Filter:
				return "\xef\x82\xb0";
			case EditorIcon::Settings:
				return "\xef\x80\x93";
			case EditorIcon::Save:
				return "\xef\x83\x87";
			case EditorIcon::Shader:
				return "\xef\x8b\x9b";
			case EditorIcon::Refresh:
				return "\xef\x80\xa1";
			case EditorIcon::Reload:
				return "\xef\x87\x80";
			case EditorIcon::Search:
				return "\xef\x80\x82";
			case EditorIcon::Level:
				return "\xef\x89\xb9";
			case EditorIcon::ViewMode:
				return "\xef\x97\xbd";
			case EditorIcon::ViewLit:
				return "\xef\x86\x85";
			case EditorIcon::ViewDiffuse:
				return "\xef\x94\xbf";
			case EditorIcon::ViewNormal:
				return "\xef\x81\x87";
			case EditorIcon::ViewRoughness:
				return "\xef\x9b\xbc";
			case EditorIcon::ViewMetallic:
				return "\xef\x81\xb6";
			case EditorIcon::ViewEmissive:
				return "\xef\x81\xad";
			case EditorIcon::ViewAmbientOcclusion:
				return "\xef\x81\x82";
			case EditorIcon::ViewSubsurfaceColor:
				return "\xef\x81\x83";
			case EditorIcon::ViewSubsurfaceStrength:
				return "\xef\x9d\xb3";
			case EditorIcon::ViewDirectDiffuse:
				return "\xef\x83\xa7";
			case EditorIcon::ViewDirectSpecular:
				return "\xef\x8e\xa5";
			case EditorIcon::ViewDirectSubsurface:
				return "\xef\x97\x92";
			case EditorIcon::Cpu:
				return "\xef\x8b\x9b";
			case EditorIcon::Gpu:
				return "\xef\x84\x88";
			case EditorIcon::Help:
				return "\xef\x81\x99";
			case EditorIcon::Clear:
				return "\xef\x87\xb8";
			case EditorIcon::Copy:
				return "\xef\x83\x85";
			case EditorIcon::Console:
				return "\xef\x84\xa0";
			case EditorIcon::SourceFile:
				return "\xef\x87\x89";
			case EditorIcon::Reflection:
				return "\xef\x82\xae";
			case EditorIcon::Disassembly:
				return "\xef\x84\xa0";
			case EditorIcon::CompileRequest:
				return "\xef\x91\xad";
			case EditorIcon::Sort:
				return "\xef\x85\xa0";
			case EditorIcon::None:
			default:
				return "\xef\x81\x99";
		}
	}

	std::string MakeIconLabel(EditorIcon icon, const char* label)
	{
		std::string result = GetEditorIconGlyph(icon);
		if (label != nullptr && label[0] != '\0')
		{
			result += ' ';
			result += label;
		}
		return result;
	}
}
