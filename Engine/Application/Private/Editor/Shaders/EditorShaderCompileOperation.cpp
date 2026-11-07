#include "PCH.h"
#include "Editor/Shaders/EditorShaderCompileOperation.h"

#include "Core/Public/Console/ConsoleOutput.h"
#include "Editor/Public/UI.h"
#include "Renderer.h"

#include <exception>
#include <utility>

EditorShaderCompileOperation::EditorShaderCompileOperation(EditorOperationRuntime& operations, const EditorShaderCompileAction& compile) :
    m_compile(compile),
    m_operation(operations)
{
}

ConsoleCommandResult EditorShaderCompileOperation::Request(bool generateSymbols)
{
	if (!m_compile)
	{
		return ConsoleCommandResult::Error("Shader compilation is unavailable in this host.");
	}

	std::string error;
	const TaskName taskName = TaskName{"Compile editor shaders"};
	auto operationTask = [compile = m_compile, generateSymbols](ConsoleCommandResult& result, TaskExecutionContext& context)
		        {
			        result = compile(generateSymbols, context.GetCancellationToken());
			        if (context.IsCancellationRequested())
			        {
				        return TaskResult::Cancelled("Shader compilation cancelled.");
			        }
			        return result.Succeeded ? TaskResult::Success() : TaskResult::Failure(result.Message);
		        };
	const bool operationStarted = m_operation.Start(taskName, "Shader compilation is already running.", std::move(operationTask), error);
	
	if (!operationStarted)
	{
		return ConsoleCommandResult::Error(std::move(error));
	}
	return ConsoleCommandResult::Success("Compiling shaders; the accepted shaders remain active until compilation and reload succeed.");
}

void EditorShaderCompileOperation::Update(UI& ui, Renderer& renderer)
{
	ConsoleCommandResult result;
	if (const auto task = m_operation.TryConsume(result))
	{
		if (!task->Succeeded())
		{
			result = ConsoleCommandResult::Error(std::string(task->GetMessage()));
		}
		else
		{
			try
			{
				renderer.ReloadShaders();
				result.Message += " Cooked shaders reloaded.";
			}
			catch (const std::exception& error)
			{
				result = ConsoleCommandResult::Error("Shader reload failed: " + std::string(error.what()));
			}
		}
		ui.SetShaderCompileStatus(false, result.Message);
		ui.AppendConsoleOutput({result.Severity, std::move(result.Message)}, !result.Succeeded);
	}
	ui.SetShaderCompileStatus(m_operation.IsOccupied());
}
