#pragma once

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Pipeline/PassPipelineRuntime.h"
#include "Pipeline/GraphicsPipelineMaterialization.h"
#include "Pipeline/RenderPassShaderRuntime.h"
#include "Pipeline/RayTracingPipelineRuntime.h"
#include "RHI/Public/Shaders/Authoring/GlobalShader.h"
#include "RHI/Public/Shaders/ShaderParameterLayoutBuilder.h"

#include <cstddef>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace RenderPassRuntimeStorage
{
	struct ShaderGeneration;

	struct Holder
	{
		virtual ~Holder() noexcept;
		Holder(const Holder&) = delete;
		Holder& operator=(const Holder&) = delete;
		Holder(Holder&&) = delete;
		Holder& operator=(Holder&&) = delete;

		virtual std::unique_ptr<Holder> CreateReplacement(RenderHardwareInterface& renderHardwareInterface, const ShaderGeneration& generation) const = 0;

	protected:
		Holder() noexcept = default;
	};

	struct ShaderGeneration final
	{
		std::uint64_t Generation = 1;
		ShaderTarget Target = kDefaultShaderTarget;
		CookedShaderLibrary Library;
		GlobalShaderMap Map;
		std::unordered_map<std::type_index, std::unique_ptr<Holder>> RuntimeStorageByType;
	};

	template <typename TShader> struct Compute final : Holder
	{
		void Materialize(RenderHardwareInterface& renderHardwareInterface, const ShaderGeneration& generation)
		{
			if (!m_runtime.has_value())
			{
				Create(renderHardwareInterface, generation);
			}
		}

		const ComputePassPipelineRuntime* Find() const noexcept { return m_runtime ? &*m_runtime : nullptr; }

		std::unique_ptr<Holder> CreateReplacement(RenderHardwareInterface& renderHardwareInterface, const ShaderGeneration& generation) const override
		{
			auto replacement = std::make_unique<Compute<TShader>>();
			replacement->Create(renderHardwareInterface, generation);
			return replacement;
		}

	private:
		void Create(RenderHardwareInterface& renderHardwareInterface, const ShaderGeneration& generation)
		{
			const ShaderRegistrationDesc& shader = GlobalShader<TShader>::GetRegistration();
			const ShaderRef<TShader> shaderRef = ShaderRef<TShader>::Resolve(generation.Map, generation.Library, generation.Target);
			RenderPassShaderRuntime::CreateComputeRuntime(renderHardwareInterface, shader, shaderRef, m_storage, m_pipeline);
			m_runtime.emplace(*m_storage.BindingLayout, *m_pipeline);
		}

		RenderPassShaderRuntimeStorage m_storage;
		std::unique_ptr<RenderPipeline> m_pipeline;
		std::optional<ComputePassPipelineRuntime> m_runtime;
	};

	template <typename TVertexShader, typename TPixelShader> struct Graphics final : Holder
	{
		void Materialize(RenderHardwareInterface& renderHardwareInterface, const ShaderGeneration& generation, const GraphicsPipelineRequest& request)
		{
			if (m_storage.BindingLayout == nullptr)
			{
				Create(renderHardwareInterface, generation);
			}
			const GraphicsPipelineKey key = BuildKey(request);
			if (m_pipelines.contains(key))
			{
				return;
			}
			MaterializePipeline(renderHardwareInterface, key);
		}

		const RenderBindingLayout* GetBindingLayout() const noexcept { return m_storage.BindingLayout.get(); }

		const RenderPipeline* FindPipeline(const GraphicsPipelineRequest& request) const noexcept
		{
			const auto pipeline = m_pipelines.find(BuildKey(request));
			return pipeline != m_pipelines.end() ? pipeline->second.get() : nullptr;
		}

		std::unique_ptr<Holder> CreateReplacement(RenderHardwareInterface& renderHardwareInterface, const ShaderGeneration& generation) const override
		{
			auto replacement = std::make_unique<Graphics<TVertexShader, TPixelShader>>();
			replacement->Create(renderHardwareInterface, generation);
			std::vector<const GraphicsPipelineKey*> requestedKeys;
			requestedKeys.reserve(m_pipelines.size());
			for (const auto& entry : m_pipelines)
			{
				requestedKeys.push_back(&entry.first);
			}
			std::ranges::sort(requestedKeys, [](const GraphicsPipelineKey* left, const GraphicsPipelineKey* right) { return GraphicsPipelineKeyHash{}(*left) < GraphicsPipelineKeyHash{}(*right); });
			for (const GraphicsPipelineKey* key : requestedKeys)
			{
				replacement->MaterializePipeline(renderHardwareInterface, replacement->BuildKey(key->Request));
			}
			return replacement;
		}

	private:
		void Create(RenderHardwareInterface& renderHardwareInterface, const ShaderGeneration& generation)
		{
			m_generation = generation.Generation;
			const ShaderRegistrationDesc& vertexShader = GlobalShader<TVertexShader>::GetRegistration();
			const ShaderRegistrationDesc& pixelShader = GlobalShader<TPixelShader>::GetRegistration();
			const ShaderRef<TVertexShader> vertexRef = ShaderRef<TVertexShader>::Resolve(generation.Map, generation.Library, generation.Target);
			const ShaderRef<TPixelShader> pixelRef = ShaderRef<TPixelShader>::Resolve(generation.Map, generation.Library, generation.Target);
			RenderPassShaderRuntime::CreateGraphicsRuntime(renderHardwareInterface, vertexShader, vertexRef, pixelShader, pixelRef, m_storage);
		}

		GraphicsPipelineKey BuildKey(const GraphicsPipelineRequest& request) const noexcept
		{
			assert(m_storage.Shaders.size() == 2 && m_storage.Shaders[0].Entry != nullptr && m_storage.Shaders[1].Entry != nullptr);
			return GraphicsPipelineKey{
			    .ShaderGeneration = m_generation,
			    .VertexShaderCode = m_storage.Shaders[0].Entry->CodeHash,
			    .PixelShaderCode = m_storage.Shaders[1].Entry->CodeHash,
			    .BindingLayout = BuildShaderParameterSignature(m_storage.ParameterLayout),
			    .Request = request};
		}

		void MaterializePipeline(RenderHardwareInterface& renderHardwareInterface, const GraphicsPipelineKey& key)
		{
			const std::size_t keyHash = GraphicsPipelineKeyHash{}(key);
			const std::size_t bucket = m_pipelines.bucket(key);
			for (auto existing = m_pipelines.begin(bucket); existing != m_pipelines.end(bucket); ++existing)
			{
				if (GraphicsPipelineKeyHash{}(existing->first) == keyHash && existing->first != key)
				{
					throw Diagnostics::Error("Distinct graphics pipeline keys produced the same hash.");
				}
			}
			std::wstring debugName = Strings::ToWide(GlobalShader<TVertexShader>::GetRegistration().ShaderName);
			debugName += L"_GraphicsKey_";
			debugName += std::to_wstring(keyHash);
			const GraphicsPipelineDesc desc = BuildGraphicsPipelineDesc(key.Request, *m_storage.BindingLayout, m_storage.Shaders[0], m_storage.Shaders[1], debugName.c_str());
			m_pipelines.emplace(key, PipelineMaterialization::CreateGraphicsPipeline(renderHardwareInterface, desc));
		}

		RenderPassShaderRuntimeStorage m_storage;
		std::uint64_t m_generation = 0;
		std::unordered_map<GraphicsPipelineKey, std::unique_ptr<RenderPipeline>, GraphicsPipelineKeyHash> m_pipelines;
	};

	template <typename TRayGenerationShader> struct RayTracing final : Holder
	{
		void Materialize(RenderHardwareInterface& renderHardwareInterface, const ShaderGeneration& generation, const RayTracingPipelineComposition& composition)
		{
			if (composition.GetRayGeneration() != GlobalShader<TRayGenerationShader>::GetRegistration().TypeId)
			{
				throw Diagnostics::Error("Ray-tracing composition ray-generation type does not match its runtime key.");
			}
			m_entries.push_back(
			    Entry{
			        .Definition = composition,
			        .Runtime = RayTracingPipelineRuntime::Create(renderHardwareInterface, generation.Map, generation.Library, generation.Target, generation.Generation, composition)});
		}

		const RayTracingPipelineRuntime* Find(const RayTracingPipelineComposition& composition) const noexcept
		{
			const auto entry = std::ranges::find(m_entries, composition, &Entry::Definition);
			return entry != m_entries.end() ? entry->Runtime.get() : nullptr;
		}

		std::unique_ptr<Holder> CreateReplacement(RenderHardwareInterface& renderHardwareInterface, const ShaderGeneration& generation) const override
		{
			auto replacement = std::make_unique<RayTracing<TRayGenerationShader>>();
			for (const Entry& entry : m_entries)
			{
				replacement->Materialize(renderHardwareInterface, generation, entry.Definition);
			}
			return replacement;
		}

	private:
		struct Entry final
		{
			RayTracingPipelineComposition Definition;
			std::unique_ptr<RayTracingPipelineRuntime> Runtime;
		};

		std::vector<Entry> m_entries;
	};

}
