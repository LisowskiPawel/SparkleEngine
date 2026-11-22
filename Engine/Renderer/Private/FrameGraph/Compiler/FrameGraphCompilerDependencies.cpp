#include "PCH.h"
#include "FrameGraphCompiler.h"

#include <algorithm>
#include <cassert>
#include <vector>

void FrameGraphCompiler::BuildResourceDependencies() noexcept
{
	for (FrameGraphPassNode& passRecord : m_plan.passes)
	{
		BuildPassResourceDependencies(passRecord);
	}
}

void FrameGraphCompiler::FinalizePassDependencies() noexcept
{
	DeduplicatePassDependencies();
	CullDeadPasses();
	BuildPassSuccessors();
	BuildTopologicalExecutionOrder();
}

void FrameGraphCompiler::DeduplicatePassDependencies() noexcept
{
	for (FrameGraphPassNode& passRecord : m_plan.passes)
	{
		auto uniqueEnd = passRecord.dependsOn.begin();

		for (const FrameGraphPassIndex dependency : passRecord.dependsOn)
		{
			if (dependency == INVALID_FRAME_GRAPH_PASS_INDEX || dependency == passRecord.index)
			{
				continue;
			}

			const auto it = std::find(passRecord.dependsOn.begin(), uniqueEnd, dependency);
			if (it == uniqueEnd)
			{
				*uniqueEnd = dependency;
				++uniqueEnd;
			}
		}

		passRecord.dependsOn.erase(uniqueEnd, passRecord.dependsOn.end());
	}
}

void FrameGraphCompiler::CullDeadPasses() noexcept
{
	for (FrameGraphPassNode& passRecord : m_plan.passes)
	{
		passRecord.bAlive = false;
	}

	for (const FrameGraphPassNode& passRecord : m_plan.passes)
	{
		if (IsRootPass(passRecord))
		{
			MarkPassAliveRecursive(passRecord.index);
		}
	}
}

void FrameGraphCompiler::MarkPassAliveRecursive(FrameGraphPassIndex passIndex) noexcept
{
	assert(passIndex < m_plan.passes.size());
	FrameGraphPassNode& passRecord = m_plan.passes[passIndex];
	if (passRecord.bAlive)
	{
		return;
	}

	passRecord.bAlive = true;
	for (const FrameGraphPassIndex dependency : passRecord.dependsOn)
	{
		if (dependency == INVALID_FRAME_GRAPH_PASS_INDEX)
		{
			continue;
		}

		MarkPassAliveRecursive(dependency);
	}
}

bool FrameGraphCompiler::IsRootPass(const FrameGraphPassNode& passRecord) const noexcept
{
	return WritesBackBuffer(passRecord) || WritesProductRoot(passRecord);
}

bool FrameGraphCompiler::WritesBackBuffer(const FrameGraphPassNode& passRecord) const noexcept
{
	for (const PassResourceDeclaration& declaration : passRecord.declarations)
	{
		if (!declaration.handle.IsValid() || !WritesToUsage(declaration.usage))
		{
			continue;
		}

		const FrameGraphResourceNode& resource = GetCompiledResourceEntry(declaration.handle);
		if (resource.kind == FrameGraphResourceKind::BackBuffer)
		{
			return true;
		}
	}

	return false;
}

bool FrameGraphCompiler::WritesProductRoot(const FrameGraphPassNode& passRecord) const noexcept
{
	for (const FrameGraphProductRoot& productRoot : m_plan.productRoots)
	{
		if (!productRoot.handle.IsValid())
		{
			continue;
		}

		const FrameGraphResourceNode& resource = GetCompiledResourceEntry(productRoot.handle);
		if (resource.lastWriterPass == passRecord.index)
		{
			return true;
		}
	}

	return false;
}

void FrameGraphCompiler::BuildPassSuccessors() noexcept
{
	for (FrameGraphPassNode& passRecord : m_plan.passes)
	{
		passRecord.successors.clear();
	}

	for (FrameGraphPassNode& passRecord : m_plan.passes)
	{
		if (!passRecord.bAlive)
		{
			continue;
		}

		for (const FrameGraphPassIndex dependency : passRecord.dependsOn)
		{
			assert(dependency < m_plan.passes.size());
			FrameGraphPassNode& dependencyPass = m_plan.passes[dependency];
			assert(dependencyPass.bAlive);
			dependencyPass.successors.push_back(passRecord.index);
		}
	}
}

void FrameGraphCompiler::BuildTopologicalExecutionOrder() noexcept
{
	m_plan.executionOrder.clear();
	m_plan.executionOrder.reserve(m_plan.passes.size());

	std::vector<std::uint32_t> remainingInDegree;
	remainingInDegree.reserve(m_plan.passes.size());
	for (const FrameGraphPassNode& passRecord : m_plan.passes)
	{
		remainingInDegree.push_back(static_cast<std::uint32_t>(passRecord.dependsOn.size()));
	}

	for (const FrameGraphPassNode& passRecord : m_plan.passes)
	{
		if (passRecord.bAlive && remainingInDegree[passRecord.index] == 0)
		{
			m_plan.executionOrder.push_back(passRecord.index);
		}
	}

	// Appending newly ready passes makes this both the traversal queue and the execution order.
	for (std::size_t executionIndex = 0; executionIndex < m_plan.executionOrder.size(); ++executionIndex)
	{
		const FrameGraphPassIndex passIndex = m_plan.executionOrder[executionIndex];
		const FrameGraphPassNode& passRecord = m_plan.passes[passIndex];
		for (const FrameGraphPassIndex successor : passRecord.successors)
		{
			assert(successor < remainingInDegree.size());
			assert(remainingInDegree[successor] > 0);
			--remainingInDegree[successor];
			if (remainingInDegree[successor] == 0 && m_plan.passes[successor].bAlive)
			{
				m_plan.executionOrder.push_back(successor);
			}
		}
	}
	assert(m_plan.executionOrder.size() == static_cast<std::size_t>(std::ranges::count_if(m_plan.passes, &FrameGraphPassNode::bAlive)) && "Frame graph contains a dependency cycle.");
}

void FrameGraphCompiler::BuildPassResourceDependencies(FrameGraphPassNode& passRecord) noexcept
{
	assert(passRecord.index < m_plan.passes.size());
	for (const PassResourceDeclaration& declaration : passRecord.declarations)
	{
		if (!declaration.handle.IsValid())
		{
			continue;
		}

		FrameGraphResourceNode& resource = GetCompiledResourceEntry(declaration.handle);
		if (IsReadOnlyUsage(declaration.usage))
		{
			RegisterReadDependency(passRecord, resource);
		}
		else if (IsReadWriteUsage(declaration.usage))
		{
			RegisterReadDependency(passRecord, resource);
			RegisterWriteDependency(passRecord, resource);
		}
		else if (IsWriteOnlyUsage(declaration.usage))
		{
			RegisterWriteDependency(passRecord, resource);
		}
		else
		{
			assert(false);
		}
	}
}

void FrameGraphCompiler::RegisterReadDependency(FrameGraphPassNode& passRecord, FrameGraphResourceNode& resource) noexcept
{
	if (resource.lastWriterPass != INVALID_FRAME_GRAPH_PASS_INDEX && resource.lastWriterPass != passRecord.index)
	{
		passRecord.dependsOn.push_back(resource.lastWriterPass);
	}

	const auto it = std::find(resource.readersSinceLastWrite.begin(), resource.readersSinceLastWrite.end(), passRecord.index);
	if (it == resource.readersSinceLastWrite.end())
	{
		resource.readersSinceLastWrite.push_back(passRecord.index);
	}
}

void FrameGraphCompiler::RegisterWriteDependency(FrameGraphPassNode& passRecord, FrameGraphResourceNode& resource) noexcept
{
	if (resource.lastWriterPass != INVALID_FRAME_GRAPH_PASS_INDEX && resource.lastWriterPass != passRecord.index)
	{
		passRecord.dependsOn.push_back(resource.lastWriterPass);
	}

	for (const FrameGraphPassIndex readerPass : resource.readersSinceLastWrite)
	{
		if (readerPass == passRecord.index)
		{
			continue;
		}

		passRecord.dependsOn.push_back(readerPass);
	}

	resource.lastWriterPass = passRecord.index;
	resource.readersSinceLastWrite.clear();
}
