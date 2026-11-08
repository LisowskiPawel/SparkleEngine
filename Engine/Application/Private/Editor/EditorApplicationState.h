#pragma once

#include "EditorApplication.h"

class GameApplication;
class UI;
class EditorOperationRuntime;
class EditorShaderCompileOperation;
class EditorViewportOutputCoordinator;

struct EditorApplication::State final
{
	std::unique_ptr<GameApplication> Game;
	std::unique_ptr<UI> Ui;
	std::unique_ptr<EditorOperationRuntime> OperationRuntime;
	std::unique_ptr<EditorShaderCompileOperation> ShaderCompileOperation;
	std::unique_ptr<EditorViewportOutputCoordinator> ViewportOutput;
};
