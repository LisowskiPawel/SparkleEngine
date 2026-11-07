#pragma once

#include "Pipeline/RenderPassRuntimeStorage.h"
#include "Renderer/Public/Diagnostics/ShaderDiagnostics.h"
#include "RHI/Public/Commands/RhiQueue.h"

#include <cassert>
#include <memory>
#include <string_view>
#include <vector>

class RenderDeviceServices;
class RenderHardwareInterface;
class RayTracingShaderTablePlan;

class RenderPassRuntimeCache final
{
public:
	explicit RenderPassRuntimeCache(RenderDeviceServices& deviceServices);
	~RenderPassRuntimeCache() noexcept;

	RenderPassRuntimeCache(const RenderPassRuntimeCache&) = delete;
	RenderPassRuntimeCache& operator=(const RenderPassRuntimeCache&) = delete;
	RenderPassRuntimeCache(RenderPassRuntimeCache&&) = delete;
	RenderPassRuntimeCache& operator=(RenderPassRuntimeCache&&) = delete;

	std::uint64_t GetShaderGeneration() const noexcept;
	std::uint64_t GetShaderPublicationHash() const noexcept;
	ShaderDiagnosticsSnapshot CaptureShaderDiagnostics() const;
	void ReloadShaders();
	void PollRetiredGenerations() noexcept;

	template <typename TShader> void MaterializeComputeShaderRuntime() const noexcept
	{
		auto& holder = GetOrCreateRuntimeStorageHolder<RenderPassRuntimeStorage::Compute<TShader>>();
		try
		{
			holder.Materialize(*m_renderHardwareInterface, *m_activeGeneration);
		}
		catch (const Diagnostics::Error& error)
		{
			HandleRuntimeCreationFailure(error.what());
		}
	}

	template <typename TShader> const ComputePassPipelineRuntime& GetComputeShaderRuntime() const noexcept
	{
		const auto* holder = FindRuntimeStorageHolder<RenderPassRuntimeStorage::Compute<TShader>>();
		const ComputePassPipelineRuntime* const runtime = holder != nullptr ? holder->Find() : nullptr;
		assert(runtime != nullptr && "Shader runtime must be materialized before recording.");
		return *runtime;
	}

	template <typename TVertexShader, typename TPixelShader> void MaterializeGraphicsShaderRuntime(const GraphicsPipelineRequest& request) const noexcept
	{
		auto& holder = GetOrCreateRuntimeStorageHolder<RenderPassRuntimeStorage::Graphics<TVertexShader, TPixelShader>>();
		try
		{
			holder.Materialize(*m_renderHardwareInterface, *m_activeGeneration, request);
		}
		catch (const Diagnostics::Error& error)
		{
			HandleRuntimeCreationFailure(error.what());
		}
	}

	template <typename TVertexShader, typename TPixelShader> RasterPassRuntime GetGraphicsShaderRuntime(const GraphicsPipelineRequest& request) const noexcept
	{
		const auto* holder = FindRuntimeStorageHolder<RenderPassRuntimeStorage::Graphics<TVertexShader, TPixelShader>>();
		if (holder == nullptr)
		{
			HandleRuntimeCreationFailure("Graphics shader runtime lookup preceded materialization.");
		}
		const RenderBindingLayout* const bindingLayout = holder->GetBindingLayout();
		const RenderPipeline* const pipeline = holder->FindPipeline(request);
		if (bindingLayout == nullptr || pipeline == nullptr)
		{
			HandleRuntimeCreationFailure("Graphics pipeline materialization did not publish the requested key.");
		}
		return RasterPassRuntime{*bindingLayout, *pipeline};
	}

	template <typename TRayGenerationShader> void MaterializeRayTracingRuntime(const RayTracingPipelineComposition& composition) const noexcept
	{
		auto& holder = GetOrCreateRuntimeStorageHolder<RenderPassRuntimeStorage::RayTracing<TRayGenerationShader>>();
		if (holder.Find(composition) == nullptr)
		{
			try
			{
				holder.Materialize(*m_renderHardwareInterface, *m_activeGeneration, composition);
			}
			catch (const Diagnostics::Error& error)
			{
				HandleRuntimeCreationFailure(error.what());
			}
		}
	}

	template <typename TRayGenerationShader> RayTracingPassPipelineRuntime GetRayTracingRuntime(const RayTracingPipelineComposition& composition) const noexcept
	{
		const auto& runtime = RequireRayTracingRuntime<TRayGenerationShader>(
		    composition,
		    "Ray-tracing runtime lookup preceded materialization.",
		    "Ray-tracing composition lookup preceded exact materialization.");

		return RayTracingPassPipelineRuntime{.BindingLayout = runtime.GetBindingLayout(), .Pipeline = runtime.GetPipeline(), .Generation = runtime.GetGeneration()};
	}

	template <typename TRayGenerationShader> std::unique_ptr<RayTracingShaderTable> CreateRayTracingShaderTable(const RayTracingPipelineComposition& composition) const noexcept
	{
		return CreateRayTracingShaderTable<TRayGenerationShader>(composition, nullptr);
	}

	template <typename TRayGenerationShader>
	std::unique_ptr<RayTracingShaderTable> CreateRayTracingShaderTable(const RayTracingPipelineComposition& composition, const RayTracingShaderTablePlan& plan) const noexcept
	{
		return CreateRayTracingShaderTable<TRayGenerationShader>(composition, &plan);
	}

private:
	struct RetiredShaderRuntimeGeneration final
	{
		RhiSubmissionState LastUse;
		std::unique_ptr<RenderPassRuntimeStorage::ShaderGeneration> Runtime;
	};

	template <typename TStorage> TStorage& GetOrCreateRuntimeStorageHolder() const noexcept
	{
		const std::type_index storageType = typeid(TStorage);
		auto& holders = m_activeGeneration->RuntimeStorageByType;
		auto runtime = holders.find(storageType);
		if (runtime == holders.end())
		{
			runtime = holders.emplace(storageType, std::make_unique<TStorage>()).first;
		}
		auto* holder = static_cast<TStorage*>(runtime->second.get());
		assert(holder != nullptr);
		return *holder;
	}

	template <typename TStorage> const TStorage* FindRuntimeStorageHolder() const noexcept
	{
		const auto& holders = m_activeGeneration->RuntimeStorageByType;
		const auto runtime = holders.find(typeid(TStorage));
		return runtime != holders.end() ? static_cast<const TStorage*>(runtime->second.get()) : nullptr;
	}

	template <typename TRayGenerationShader>
	const RayTracingPipelineRuntime& RequireRayTracingRuntime(const RayTracingPipelineComposition& composition, std::string_view missingHolder, std::string_view missingComposition) const noexcept
	{
		const auto* holder = FindRuntimeStorageHolder<RenderPassRuntimeStorage::RayTracing<TRayGenerationShader>>();
		if (holder == nullptr)
		{
			HandleRuntimeCreationFailure(missingHolder);
		}
		const auto* runtime = holder->Find(composition);
		if (runtime == nullptr)
		{
			HandleRuntimeCreationFailure(missingComposition);
		}
		return *runtime;
	}

	template <typename TRayGenerationShader>
	std::unique_ptr<RayTracingShaderTable> CreateRayTracingShaderTable(const RayTracingPipelineComposition& composition, const RayTracingShaderTablePlan* plan) const noexcept
	{
		const auto& runtime = RequireRayTracingRuntime<TRayGenerationShader>(
		    composition,
		    "Ray-tracing shader-table creation preceded pipeline materialization.",
		    "Ray-tracing shader-table creation has no exact pipeline composition.");

		return CreateRayTracingShaderTable(runtime, composition, plan);
	}

	std::unique_ptr<RayTracingShaderTable> CreateRayTracingShaderTable(
	    const RayTracingPipelineRuntime& runtime,
	    const RayTracingPipelineComposition& composition,
	    const RayTracingShaderTablePlan* plan) const noexcept;
	RhiSubmissionState CaptureLastSubmittedState() const noexcept;
	std::unique_ptr<RenderPassRuntimeStorage::ShaderGeneration> OpenGeneration(std::uint64_t generation) const;
	std::unique_ptr<RenderPassRuntimeStorage::ShaderGeneration> CreateReplacementGeneration(std::uint64_t generation) const;
	void ActivateGeneration(std::unique_ptr<RenderPassRuntimeStorage::ShaderGeneration> replacement);
	static void ValidateGenerationContracts(const RenderPassRuntimeStorage::ShaderGeneration& generation);
	bool IsComplete(const RhiSubmissionState& state) const noexcept;
	[[noreturn]] void HandleRuntimeCreationFailure(std::string_view errorMessage) const;

	RenderDeviceServices* m_deviceServices = nullptr;
	RenderHardwareInterface* m_renderHardwareInterface = nullptr;
	mutable std::unique_ptr<RenderPassRuntimeStorage::ShaderGeneration> m_activeGeneration;
	std::vector<RetiredShaderRuntimeGeneration> m_retiredGenerations;
};
