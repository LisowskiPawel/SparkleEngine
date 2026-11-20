#include "PCH.h"

#include "FrameGraph/FrameGraph.h"

#include "Commands/RenderCommandContext.h"
#include "Core/Public/Diagnostics/Verify.h"

#include <cassert>
#include <format>
#include <string>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogFrameGraphBarrierPlayback, "Renderer.FrameGraph");

static std::string FormatResourceLabel(const FrameGraphResourceHandle handle) noexcept
{
	if (!handle.IsValid())
	{
		return "invalid";
	}

	return std::format("{}", handle.index);
}

static void FailUnresolvedAliasingBarrier(std::string_view passName, const FrameGraphAliasingBarrier& barrier, std::string_view beforeResourceName, std::string_view afterResourceName) noexcept
{
	const std::string message = std::format(
	    "FrameGraph aliasing barrier validation failed: pass='{}' block={} beforeHandle={} beforeResource='{}' afterHandle={} "
	    "afterResource='{}'",
	    passName,
	    barrier.physicalBlockIndex,
	    FormatResourceLabel(barrier.beforeHandle),
	    beforeResourceName,
	    FormatResourceLabel(barrier.afterHandle),
	    afterResourceName);

	Diagnostics::Fatal(LogFrameGraphBarrierPlayback, __FILE__, __LINE__, message);
}

static void FailUnresolvedResourceBarrier(std::string_view passName, const FrameGraphBarrier& barrier, std::string_view resourceName) noexcept
{
	const std::string message = std::format(
	    "FrameGraph resource barrier failed: pass='{}' handle={} resource='{}' label='{}' beforeState={} afterState={}",
	    passName,
	    FormatResourceLabel(barrier.handle),
	    resourceName,
	    barrier.label.empty() ? "<unlabeled>" : barrier.label,
	    ResourceStateToString(barrier.before),
	    ResourceStateToString(barrier.after));

	Diagnostics::Fatal(LogFrameGraphBarrierPlayback, __FILE__, __LINE__, message);
}

void FrameGraph::EmitTransientAliasingBarriers(RenderCommandContext& commandContext, std::string_view passName, const std::vector<FrameGraphAliasingBarrier>& barriers) const noexcept
{
	for (const FrameGraphAliasingBarrier& barrier : barriers)
	{
		assert(barrier.beforeHandle.IsValid());
		assert(barrier.afterHandle.IsValid());

		const RhiResourceHandle beforeResource = ResolveResource(barrier.beforeHandle);
		const RhiResourceHandle afterResource = ResolveResource(barrier.afterHandle);

		if (!beforeResource || !afterResource)
		{
			std::string_view beforeName = "<unknown>";
			if (m_resourceRegistry.IsRegistered(barrier.beforeHandle))
			{
				beforeName = m_resourceRegistry.GetMetadata(barrier.beforeHandle).debugName;
			}

			std::string_view afterName = "<unknown>";
			if (m_resourceRegistry.IsRegistered(barrier.afterHandle))
			{
				afterName = m_resourceRegistry.GetMetadata(barrier.afterHandle).debugName;
			}

			FailUnresolvedAliasingBarrier(passName, barrier, beforeName, afterName);
		}

		commandContext.AliasResource(beforeResource, afterResource);
	}
}

void FrameGraph::EmitCompiledBarriers(RenderCommandContext& commandContext, std::string_view passName, const std::vector<FrameGraphBarrier>& barriers) const noexcept
{
	for (const FrameGraphBarrier& barrier : barriers)
	{
		const RhiResourceHandle resource = ResolveResource(barrier.handle);
		if (!resource)
		{
			std::string_view resourceName = "<unknown>";
			if (m_resourceRegistry.IsRegistered(barrier.handle))
			{
				resourceName = m_resourceRegistry.GetMetadata(barrier.handle).debugName;
			}

			FailUnresolvedResourceBarrier(passName, barrier, resourceName);
		}

		switch (barrier.type)
		{
			case FrameGraphBarrier::Type::Transition:
				commandContext.TransitionResource(resource, barrier.before, barrier.after);
				break;
			case FrameGraphBarrier::Type::UnorderedAccess:
			case FrameGraphBarrier::Type::AccelerationStructure:
				commandContext.UnorderedAccessBarrier(resource);
				break;
			default:
				assert(false);
				break;
		}
	}
}
