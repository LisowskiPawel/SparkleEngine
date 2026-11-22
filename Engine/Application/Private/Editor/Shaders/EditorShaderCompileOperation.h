#pragma once

#include "EditorShaderCompileAction.h"
#include <memory>
#include <stop_token>

struct enkiTaskScheduler;

class UI;
class Renderer;

class EditorShaderCompileOperation final
{
public:
	EditorShaderCompileOperation(enkiTaskScheduler* io, std::stop_token stop, const EditorShaderCompileAction& compile) noexcept;
	~EditorShaderCompileOperation();
	void Shutdown() noexcept;
	ConsoleCommandResult Request(bool bGenerateSymbols);
	void Update(UI& ui, Renderer& renderer);

private:
	const EditorShaderCompileAction& m_compile;
	struct Compile;
	static void CompileShaders(void* argument) noexcept;
	enkiTaskScheduler* m_io;
	std::stop_token m_stop;
	std::unique_ptr<Compile> m_operation;
};
