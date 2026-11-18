#include "PCH.h"

#include "RayTracing/RhiRayTracingPipelineDesc.h"
#include "Shaders/ResolvedShader.h"
#include "Validation/RhiContract.h"
#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"

#include <format>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_rayTracingLogger, "RHI.RayTracing");

RayTracingPipeline::RayTracingPipeline(const RayTracingPipelineDesc& desc) :
    m_generation(desc.Generation)
{
	RhiContract::ValidateRayTracingPipelineDesc(desc);
	m_recordContracts.reserve(desc.ShaderExports.size() + desc.HitGroups.size());
	for (const RhiRayTracingShaderExportDesc& shaderExport : desc.ShaderExports)
	{
		const GlobalShaderMapEntry& entry = *shaderExport.Shader->Entry;

		m_recordContracts.push_back(
		    RecordContract{
		        .ExportName = std::string(shaderExport.ExportName),
		        .Stage = entry.Stage,
		        .LocalRecordSizeInBytes = entry.LocalRecordSizeInBytes,
		        .LocalRecordSignature = entry.LocalRecordSignature});
	}
	for (const RhiRayTracingHitGroupDesc& hitGroup : desc.HitGroups)
	{
		const RecordContract* closestHit = FindRecordContract(hitGroup.ClosestHitExport);

		m_recordContracts.push_back(
		    RecordContract{
		        .ExportName = std::string(hitGroup.ExportName),
		        .LocalRecordSizeInBytes = closestHit != nullptr ? closestHit->LocalRecordSizeInBytes : 0,
		        .LocalRecordSignature = closestHit != nullptr ? closestHit->LocalRecordSignature : 0,
		        .bIsHitGroup = true});
	}
}

RayTracingPipeline::~RayTracingPipeline() noexcept = default;

const RayTracingPipeline::RecordContract* RayTracingPipeline::FindRecordContract(std::string_view exportName) const noexcept
{
	for (const RecordContract& contract : m_recordContracts)
	{
		if (contract.ExportName == exportName)
		{
			return &contract;
		}
	}
	return nullptr;
}

RayTracingShaderTable::RayTracingShaderTable(const RayTracingShaderTableDesc& desc) :
    m_generation(desc.Generation),
    m_pipeline(desc.Pipeline)
{
	RhiContract::ValidateRayTracingShaderTableDesc(desc);
}

RayTracingShaderTable::~RayTracingShaderTable() noexcept = default;

bool RayTracingShaderTable::MatchesPipeline(const RayTracingPipeline& pipeline) const noexcept
{
	return m_pipeline == &pipeline && m_generation == pipeline.GetGeneration();
}

void RhiContract::ValidateTraceRaysDesc(const TraceRaysDesc& desc, ERhiQueueType queueType) noexcept
{
	if (queueType != ERhiQueueType::Graphics || desc.Pipeline == nullptr || desc.ShaderTable == nullptr)
	{
		Diagnostics::Fatal(g_rayTracingLogger, __FILE__, __LINE__, "TraceRays requires a graphics queue, pipeline and shader table.");
	}
	if (desc.Width == 0 || desc.Height == 0 || desc.Depth == 0)
	{
		Diagnostics::Fatal(g_rayTracingLogger, __FILE__, __LINE__, "TraceRays dimensions must be nonzero.");
	}
	if (!desc.ShaderTable->MatchesPipeline(*desc.Pipeline))
	{
		Diagnostics::Fatal(
		    g_rayTracingLogger,
		    __FILE__,
		    __LINE__,
		    std::format("TraceRays shader table belongs to a different pipeline or generation (table={}, pipeline={}).", desc.ShaderTable->GetGeneration(), desc.Pipeline->GetGeneration()));
	}
	const bool bGeneralRegionsMatch = desc.RayGeneration == desc.ShaderTable->GetRayGenerationRegion() && desc.Miss == desc.ShaderTable->GetMissRegion();
	const bool bOtherRegionsMatch = desc.HitGroup == desc.ShaderTable->GetHitGroupRegion() && desc.Callable == desc.ShaderTable->GetCallableRegion();
	if (!bGeneralRegionsMatch || !bOtherRegionsMatch)
	{
		Diagnostics::Fatal(g_rayTracingLogger, __FILE__, __LINE__, "TraceRays regions do not match the immutable shader table.");
	}
}
