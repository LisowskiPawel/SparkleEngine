#pragma once

#include "EditorShaderCompileAction.h"
#include "EditorOperations/EditorOperationSlot.h"

class UI;
class Renderer;

class EditorShaderCompileOperation final
{
public:
	explicit EditorShaderCompileOperation(EditorOperationRuntime& operations, const EditorShaderCompileAction& compile);
	ConsoleCommandResult Request(bool bGenerateSymbols);
	void Update(UI& ui, Renderer& renderer);

private:
	const EditorShaderCompileAction& m_compile;
	EditorOperationSlot<ConsoleCommandResult> m_operation;
};
