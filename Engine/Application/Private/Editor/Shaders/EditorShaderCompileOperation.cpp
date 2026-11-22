#include "PCH.h"
#include "Editor/Shaders/EditorShaderCompileOperation.h"

#include "Core/Public/Console/ConsoleOutput.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Editor/Public/UI.h"
#include "Renderer.h"
#include <TaskScheduler_c.h>
#include <exception>
#include <new>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_editorShaderLogger, "Application.ShaderCompile");

struct EditorShaderCompileOperation::Compile final
{
	EditorShaderCompileAction Action;
	bool bGenerateSymbols;
	std::stop_token Stop;
	ConsoleCommandResult Result;
	enkiPinnedTask* Task = nullptr;
};

EditorShaderCompileOperation::EditorShaderCompileOperation(enkiTaskScheduler* io, std::stop_token stop, const EditorShaderCompileAction& compile) noexcept :
    m_compile(compile),
    m_io(io),
    m_stop(stop)
{
}

EditorShaderCompileOperation::~EditorShaderCompileOperation()
{
	if (m_operation)
	{
		Diagnostics::Fatal(g_editorShaderLogger, __FILE__, __LINE__, "Shader compile work must be consumed or explicitly shut down before owner destruction.");
	}
}

void EditorShaderCompileOperation::Shutdown() noexcept
{
	if (!m_operation)
	{
		return;
	}
	if (m_operation->Task)
	{
		enkiWaitForPinnedTask(m_io, m_operation->Task);
		enkiDeletePinnedTask(m_io, m_operation->Task);
	}
	m_operation.reset();
}

void EditorShaderCompileOperation::CompileShaders(void* argument) noexcept
{
	auto& compile = *static_cast<Compile*>(argument);
	try
	{
		if (!compile.Stop.stop_requested())
		{
			compile.Result = compile.Action(compile.bGenerateSymbols, compile.Stop);
		}
		if (compile.Stop.stop_requested())
		{
			compile.Result = ConsoleCommandResult::Error("Shader compilation cancelled.");
		}
	}
	catch (const std::exception& error)
	{
		compile.Result = ConsoleCommandResult::Error(error.what());
	}
	catch (...)
	{
		compile.Result = ConsoleCommandResult::Error("Unknown shader compilation failure.");
	}
}

ConsoleCommandResult EditorShaderCompileOperation::Request(bool bGenerateSymbols)
{
	if (!m_compile)
	{
		return ConsoleCommandResult::Error("Shader compilation is unavailable in this host.");
	}
	if (m_operation)
	{
		return ConsoleCommandResult::Error("Shader compilation is already running.");
	}
	if (m_stop.stop_requested())
	{
		return ConsoleCommandResult::Error("Shader compilation owner is closed.");
	}
	try
	{
		auto compile = std::make_unique<Compile>(m_compile, bGenerateSymbols, m_stop);
		if (m_io)
		{
			const auto config = enkiGetTaskSchedulerConfig(m_io);
			const auto worker = config.numExternalTaskThreads + 1 + 2 % config.numTaskThreadsToCreate;
			compile->Task = enkiCreatePinnedTask(m_io, CompileShaders, worker);
			if (!compile->Task)
			{
				throw std::bad_alloc();
			}
			enkiSetArgsPinnedTask(compile->Task, compile.get());
		}
		m_operation = std::move(compile);
		if (m_io)
		{
			enkiAddPinnedTask(m_io, m_operation->Task);
		}
		else
		{
			CompileShaders(m_operation.get());
		}
	}
	catch (const std::exception& error)
	{
		return ConsoleCommandResult::Error(error.what());
	}
	return ConsoleCommandResult::Success("Compiling shaders; the accepted shaders remain active until compilation and reload succeed.");
}

void EditorShaderCompileOperation::Update(UI& ui, Renderer& renderer)
{
	const bool bSettled = !m_operation || !m_operation->Task;
	const bool bComplete = bSettled || enkiIsPinnedTaskComplete(m_io, m_operation->Task);
	if (m_operation && bComplete)
	{
		auto result = std::move(m_operation->Result);
		if (m_operation->Task)
		{
			enkiDeletePinnedTask(m_io, m_operation->Task);
		}
		m_operation.reset();
		if (result.bSucceeded)
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
		ui.AppendConsoleOutput({result.Severity, std::move(result.Message)}, !result.bSucceeded);
	}
	ui.SetShaderCompileStatus(m_operation != nullptr);
}
