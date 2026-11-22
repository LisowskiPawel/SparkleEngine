#include "PCH.h"
#include "Textures/TextureCache.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "RHI/Public/Commands/RenderCommandList.h"
#include "RHI/Public/Resources/RhiResourceService.h"
#include "Textures/CookedTextureLoader.h"

#include <algorithm>
#include <format>
#include <string>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_textureCacheLogger, "Renderer.TextureCache");

void TextureCache::LoadDefaultTextures(RenderCommandList& commandList, std::vector<RhiResourceHandle>& uploadedResources)
{
	if (m_bDefaultsLoaded)
	{
		return;
	}

	for (std::uint8_t index = 0; index < static_cast<std::uint8_t>(DefaultTexture::Count); ++index)
	{
		const DefaultTexture type = static_cast<DefaultTexture>(index);
		const std::filesystem::path& texturePath = DefaultTextures::GetPath(type);
		const std::optional<ResolvedTexturePath> source = ResolveTexturePath(texturePath);
		if (!source)
		{
			const std::string message = std::format("Default texture '{}' could not be resolved.", texturePath.string());

			Diagnostics::Fatal(g_textureCacheLogger, __FILE__, __LINE__, message);
		}

		if (m_defaultTextures.contains(source->CacheKey))
		{
			continue;
		}

		LoadedTextureData decodedTexture;
		try
		{
			const auto maximumDecodedBytes = m_residency.GetBudget().MaximumDecodedBytes;
			const CookedTextureFilePayload payload = ReadCookedTexturePayload(source->Path, maximumDecodedBytes);
			decodedTexture = DecodeCookedTexture(payload, maximumDecodedBytes);
		}
		catch (const Diagnostics::Error& error)
		{
			const std::string message = std::format("Default texture '{}' failed to load: {}", source->Path.string(), error.what());

			Diagnostics::Fatal(g_textureCacheLogger, __FILE__, __LINE__, message);
		}

		RendererTexture texture = m_textureFactory.Create(source->Path, decodedTexture, commandList);
		uploadedResources.push_back(m_resourceService.GetResourceHandle(texture.Resource));
		m_defaultTextures.emplace(source->CacheKey, texture);
	}

	const std::optional<ResolvedTexturePath> checker = ResolveTexturePath(DefaultTextures::GetPath(DefaultTexture::Checkerboard));
	const std::optional<ResolvedTexturePath> sky = ResolveTexturePath(DefaultTextures::GetPath(DefaultTexture::Sky));
	if (!checker || !sky)
	{
		Diagnostics::Fatal(g_textureCacheLogger, __FILE__, __LINE__, "Checker or sky texture slot could not be resolved.");
	}
	m_textureSlotKeys[static_cast<std::size_t>(TextureId::Checker)] = checker->CacheKey;
	m_textureSlotKeys[static_cast<std::size_t>(TextureId::DefaultSky)] = sky->CacheKey;
	m_bDefaultsLoaded = true;
	++m_bindingRevision;
}

void TextureCache::RequestTexture(const ResolvedTexturePath& source, std::uint32_t generation)
{
	if (m_defaultTextures.contains(source.CacheKey))
	{
		return;
	}
	const auto active = m_pathTextures.find(source.CacheKey);
	if (active != m_pathTextures.end() && active->second.Generation.Generation == generation)
	{
		return;
	}
	if (FindRequest(source.CacheKey, generation) != nullptr)
	{
		return;
	}

	const auto& budget = m_residency.GetBudget();
	const auto maximumDecodedBytes = std::min(budget.MaximumDecodedBytes, budget.MaximumPendingUploadBytes);
	const auto decodeReservationBytes = GetCookedTextureDecodeReservation(source.Path, maximumDecodedBytes);
	const std::optional<AssetGenerationHandle> handle = m_residency.BeginGeneration(MakeAssetKey(source.CacheKey), generation);
	if (!handle)
	{
		Diagnostics::Fatal(g_textureCacheLogger, __FILE__, __LINE__, "Texture generation could not enter residency.");
	}

	TextureRequest request;
	request.Source = source;
	request.Generation = *handle;
	request.DecodeReservationBytes = decodeReservationBytes;
	m_requests.push_back(std::move(request));
	LaunchPendingRequests();
}

void TextureCache::CancelRequest(TextureRequest& request) noexcept
{
	request.bWanted = false;
	if (request.Load)
	{
		request.Load->Cancel();
		return;
	}
	(void) m_residency.Cancel(request.Generation);
	request.Decoded.reset();
	request.Load.reset();
}

void TextureCache::SynchronizeSceneTextures(const RenderTextureTable& textures)
{
	if (textures.Generation == 0)
	{
		Diagnostics::Fatal(g_textureCacheLogger, __FILE__, __LINE__, "Scene texture table has no generation identity.");
	}
	const std::uint32_t generation = textures.Generation;
	m_wantedPathTextureKeys.clear();
	for (const RenderTextureAsset& texture : textures.Assets)
	{
		const std::optional<ResolvedTexturePath> source = ResolveTexturePath(texture.Path);
		if (!source)
		{
			const std::string message = std::format("Cooked scene texture '{}' could not be resolved.", texture.Path.string());

			Diagnostics::Fatal(g_textureCacheLogger, __FILE__, __LINE__, message);
		}

		m_wantedPathTextureKeys.insert(source->CacheKey);
		RequestTexture(*source, generation);
	}

	for (TextureRequest& request : m_requests)
	{
		request.bWanted = request.Generation.Generation == generation && m_wantedPathTextureKeys.contains(request.Source.CacheKey);
		if (!request.bWanted)
		{
			CancelRequest(request);
		}
	}

	bool bRetiredTexture = false;
	const std::uint64_t nextBindingRevision = m_bindingRevision + 1u;
	for (auto texture = m_pathTextures.begin(); texture != m_pathTextures.end();)
	{
		const bool bCurrentGeneration = texture->second.Generation.Generation == generation;
		if (bCurrentGeneration && m_wantedPathTextureKeys.contains(texture->first))
		{
			++texture;
			continue;
		}

		QueueTextureRetirement(texture->second, nextBindingRevision);
		texture = m_pathTextures.erase(texture);
		bRetiredTexture = true;
	}

	if (bRetiredTexture || m_sceneTextureGeneration != generation)
	{
		m_bindingRevision = nextBindingRevision;
	}
	m_sceneTextureGeneration = generation;
}

void TextureCache::LaunchPendingRequests()
{
	const std::size_t activeLoadCount = static_cast<std::size_t>(std::count_if(m_requests.begin(), m_requests.end(), [](const TextureRequest& request) noexcept { return request.Load != nullptr; }));

	std::size_t availableSlots = activeLoadCount < kMaximumConcurrentLoads ? kMaximumConcurrentLoads - activeLoadCount : 0;
	for (std::size_t requestIndex = 0; requestIndex < m_requests.size(); ++requestIndex)
	{
		TextureRequest& request = m_requests[requestIndex];
		if (availableSlots == 0)
		{
			return;
		}
		const bool bPending = m_residency.GetState(request.Generation) == AssetResidencyState::Reading && request.bWanted;
		if (request.Load || !bPending)
		{
			continue;
		}

		if (!m_residency.BeginDecoding(request.Generation, request.DecodeReservationBytes))
		{
			continue;
		}
		try
		{
			auto load = std::make_unique<CookedTextureLoad>(m_io, m_cpu, request.Source.Path, request.DecodeReservationBytes);
			load->Start(static_cast<std::uint32_t>(requestIndex));
			request.Load = std::move(load);
		}
		catch (...)
		{
			(void) m_residency.Cancel(request.Generation);
			throw;
		}
		--availableSlots;
	}
}

void TextureCache::ConsumeCompletedRequests() noexcept
{
	bool bAcceptedPayload = false;
	for (TextureRequest& request : m_requests)
	{
		if (!request.Load || !request.Load->IsSettled())
		{
			continue;
		}

		if (!request.bWanted)
		{
			(void) m_residency.Cancel(request.Generation);
			request.Load.reset();
			continue;
		}
		LoadedTextureData texture;
		try
		{
			texture = request.Load->TakeTexture();
		}
		catch (const std::exception& error)
		{
			const std::string message = std::format("Texture generation {} for '{}' failed: {}", request.Generation.Generation, request.Source.Path.string(), error.what());
			Diagnostics::Fatal(g_textureCacheLogger, __FILE__, __LINE__, message);
		}
		catch (...)
		{
			Diagnostics::Fatal(g_textureCacheLogger, __FILE__, __LINE__, "Unknown texture decode failure.");
		}
		request.Load.reset();

		const std::uint64_t decodedBytes = RendererTextureFactory::CalculatePayloadBytes(texture.Upload);
		if (!m_residency.PublishReadyForUpload(request.Generation, decodedBytes, decodedBytes))
		{
			Diagnostics::Fatal(g_textureCacheLogger, __FILE__, __LINE__, "Texture generation exceeded reserved residency capacity.");
		}

		request.Decoded.emplace(std::move(texture));
		bAcceptedPayload = true;
	}
	if (bAcceptedPayload)
	{
		ValidateSceneTextureCapacity();
	}

	m_requests.erase(
	    std::remove_if(
	        m_requests.begin(),
	        m_requests.end(),
	        [this](const TextureRequest& request) noexcept
	        {
		        const AssetResidencyState state = m_residency.GetState(request.Generation);
		        return !request.Load && !request.Decoded && !request.Uploaded && state == AssetResidencyState::Retired;
	        }),
	    m_requests.end());
}

void TextureCache::ValidateSceneTextureCapacity() const noexcept
{
	std::uint64_t wantedBytes = 0;
	for (const auto& [key, texture] : m_pathTextures)
	{
		if (texture.Generation.Generation == m_sceneTextureGeneration && m_wantedPathTextureKeys.contains(key))
		{
			wantedBytes += texture.Texture.EstimatedByteSize;
		}
	}
	for (const TextureRequest& request : m_requests)
	{
		if (request.bWanted)
		{
			if (const auto* generation = m_residency.Find(request.Generation))
			{
				wantedBytes += generation->UploadBytes;
			}
		}
	}
	const auto capacity = m_residency.GetBudget().MaximumResidentBytes;
	if (wantedBytes > capacity)
	{
		const auto message = std::format(
		    "Scene texture generation {} requires at least {} bytes, exceeding its {}-byte residency budget. Reduce texture storage or resolution.",
		    m_sceneTextureGeneration,
		    wantedBytes,
		    capacity);
		Diagnostics::Fatal(g_textureCacheLogger, __FILE__, __LINE__, message);
	}
}
