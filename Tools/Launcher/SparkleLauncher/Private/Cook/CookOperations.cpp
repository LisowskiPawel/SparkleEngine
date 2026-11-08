#include "SparkleLauncher/CookOperations.h"

#include <algorithm>

namespace SparkleLauncher
{
	std::string ToString(CookOperationKind kind)
	{
		switch (kind)
		{
			case CookOperationKind::CookWorkspace:
				return "CookWorkspace";
			case CookOperationKind::CookShaders:
				return "CookShaders";
			case CookOperationKind::BuildTextures:
				return "BuildTextures";
			case CookOperationKind::BuildSceneAssets:
				return "BuildSceneAssets";
			case CookOperationKind::CookAllAssets:
				return "CookAllAssets";
		}

		return "Unknown";
	}

	std::string ToString(CookWorkspaceScope scope)
	{
		switch (scope)
		{
			case CookWorkspaceScope::Shaders:
				return "shaders";
			case CookWorkspaceScope::Textures:
				return "textures";
			case CookWorkspaceScope::SceneAssets:
				return "assets";
		}

		return "unknown";
	}

	std::string DisplayName(CookWorkspaceScope scope)
	{
		switch (scope)
		{
			case CookWorkspaceScope::Shaders:
				return "Shaders";
			case CookWorkspaceScope::Textures:
				return "Textures";
			case CookWorkspaceScope::SceneAssets:
				return "Scene assets";
		}

		return "Unknown";
	}

	bool TryParseCookWorkspaceScope(std::string_view value, CookWorkspaceScope& outScope)
	{
		if (value == "shaders")
		{
			outScope = CookWorkspaceScope::Shaders;
			return true;
		}
		if (value == "textures")
		{
			outScope = CookWorkspaceScope::Textures;
			return true;
		}
		if (value == "assets" || value == "scene-assets")
		{
			outScope = CookWorkspaceScope::SceneAssets;
			return true;
		}
		return false;
	}

	std::string ToString(CookMode mode)
	{
		switch (mode)
		{
			case CookMode::Incremental:
				return "incremental";
			case CookMode::Force:
				return "force";
		}

		return "unknown";
	}

	const std::vector<CookOperationDefinition>& GetCookOperationDefinitions()
	{
		static const std::vector<CookOperationDefinition> definitions = {
		    {CookOperationKind::CookWorkspace, "cook.workspace", "Cook", "Cook Workspace", "Cook the selected shader, texture, and scene outputs as one request."},
		    {CookOperationKind::CookAllAssets, "cook.all", "Cook", "Cook All", "Prepare all selected level assets."},
		    {CookOperationKind::CookShaders, "cook.shaders", "Cook", "Cook Shaders", "Validate and prepare the shader map and code library."},
		    {CookOperationKind::BuildTextures, "cook.textures", "Cook", "Cook Textures", "Prepare texture assets for Editor and Game."},
		    {CookOperationKind::BuildSceneAssets, "cook.assets", "Cook", "Cook Scenes And Meshes", "Prepare scene, mesh, and material assets for Editor and Game."},
		};

		return definitions;
	}

	std::optional<CookOperationDefinition> FindCookOperationDefinition(std::string_view operationId)
	{
		const std::vector<CookOperationDefinition>& definitions = GetCookOperationDefinitions();
		const auto found = std::find_if(definitions.begin(), definitions.end(), [operationId](const CookOperationDefinition& definition) { return definition.Id == operationId; });
		return found == definitions.end() ? std::nullopt : std::optional<CookOperationDefinition>(*found);
	}

}
