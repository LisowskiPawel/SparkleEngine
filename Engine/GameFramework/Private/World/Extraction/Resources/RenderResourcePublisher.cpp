#include "PCH.h"
#include "World/Extraction/Resources/RenderResourcePublisher.h"

#include "GameFramework/Public/World/SkyEnvironment.h"
#include "World/Resources/MaterialResourceStore.h"
#include "World/Resources/TextureResourceStore.h"
#include "World/SceneMeshInstanceData.h"

namespace ECS
{
	void RenderResourcePublisher::BeginScene() noexcept
	{
		m_materialRevision = 0;
		m_textureRevision = 0;
		m_skyTexturePath.reset();
		m_sky.reset();
		m_bSkyPublished = false;
	}

	void RenderResourcePublisher::Publish(
	    std::span<const SceneMeshInstanceGroupData> instanceGroups,
	    const std::optional<SkyEnvironment>& sky,
	    const MaterialResourceStore& materials,
	    const TextureResourceStore& textures,
	    RenderSceneDelta& delta)
	{
		PublishInstanceGroups(instanceGroups, delta);
		PublishSky(sky, delta);

		std::optional<std::filesystem::path> skyTexturePath;
		if (sky && sky->skyTexture.IsValid())
		{
			skyTexturePath.emplace(sky->skyTexture.texturePath);
		}
		PublishChangedTables(materials, textures, skyTexturePath, delta);
	}

	void RenderResourcePublisher::PublishInstanceGroups(std::span<const SceneMeshInstanceGroupData> instanceGroups, RenderSceneDelta& delta) const
	{
		if (!delta.bResetScene)
		{
			return;
		}
		delta.InstanceGroups.bPublished = true;
		delta.InstanceGroups.Values.reserve(instanceGroups.size());
		for (const SceneMeshInstanceGroupData& group : instanceGroups)
		{
			delta.InstanceGroups.Values.push_back({group.meshAssetId, group.meshAssetIndex, group.materialHandle, group.firstInstance, group.instanceCount, group.groupKind, group.flags});
		}
	}

	void RenderResourcePublisher::PublishSky(const std::optional<SceneSkyDesc>& sky, RenderSceneDelta& delta)
	{
		if (!delta.bResetScene && m_bSkyPublished && HasSameSky(sky, m_sky))
		{
			return;
		}
		delta.Sky.bPublished = true;
		delta.Sky.Value = sky;
		m_sky = sky;
		m_bSkyPublished = true;
	}

	void RenderResourcePublisher::PublishChangedTables(
	    const MaterialResourceStore& materials,
	    const TextureResourceStore& textures,
	    const std::optional<std::filesystem::path>& skyTexturePath,
	    RenderSceneDelta& delta)
	{
		const std::uint64_t materialRevision = materials.GetContentRevision();
		if (delta.bResetScene || materialRevision != m_materialRevision)
		{
			delta.Materials = materials.CaptureRenderTable();
			m_materialRevision = materialRevision;
		}

		const std::uint64_t textureRevision = textures.GetContentRevision();
		if (delta.bResetScene || textureRevision != m_textureRevision || skyTexturePath != m_skyTexturePath)
		{
			if (skyTexturePath)
			{
				delta.Textures = textures.CaptureRenderTable(std::span<const std::filesystem::path>(&*skyTexturePath, 1));
			}
			else
			{
				delta.Textures = textures.CaptureRenderTable();
			}
			m_textureRevision = textureRevision;
			m_skyTexturePath = skyTexturePath;
		}
	}

	bool RenderResourcePublisher::HasSameSky(const std::optional<SceneSkyDesc>& left, const std::optional<SceneSkyDesc>& right) noexcept
	{
		if (left.has_value() != right.has_value())
		{
			return false;
		}
		if (!left)
		{
			return true;
		}

		return left->bEnabled == right->bEnabled && left->color.x == right->color.x && left->color.y == right->color.y && left->color.z == right->color.z && left->brightness == right->brightness
		    && left->skyTexture.texturePath == right->skyTexture.texturePath && left->skyTexture.textureGroup == right->skyTexture.textureGroup;
	}
}
