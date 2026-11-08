#pragma once

#include <cstdint>

class TaskExecutor;
class TaskScope;
struct RendererExecutionConfig;

namespace ConcurrencyLaunchCVars
{
	void Register() noexcept;
	RendererExecutionConfig ResolveRendererExecution(bool bAllowThreaded, bool bEnableUiRenderPackets, TaskExecutor& executor, TaskScope& parentScope) noexcept;
}
