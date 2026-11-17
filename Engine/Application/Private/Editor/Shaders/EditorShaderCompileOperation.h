#pragma once

#include "EditorShaderCompileAction.h"
#include "Editor/Operations/EditorOperationSlot.h"

class UI;
class Renderer;

class EditorShaderCompileOperation final
{
public:
	EditorShaderCompileOperation(TaskExecutor& executor, TaskScope& scope, const EditorShaderCompileAction& compile);
	ConsoleCommandResult Request(bool bGenerateSymbols);
	void Update(UI& ui, Renderer& renderer);

private:
	const EditorShaderCompileAction& m_compile;
	EditorOperationSlot<ConsoleCommandResult> m_operation;
};
