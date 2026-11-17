#include "Cli/ShaderCompilerCli.h"
#include "Core/Public/Threading/ThreadOwnership.h"

int main(int argc, char** argv)
{
	Threading::SetCurrentThreadRole("Sparkle.ToolMain");

	return RunShaderCompiler(argc, argv);
}
