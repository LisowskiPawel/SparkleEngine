#include "EditorApplicationLaunch.h"
#if SPARKLE_EDITOR_WITH_SHADER_COMPILATION
  #include "SparkleLauncher/EditorShaderCompilation.h"
#endif

int wmain()
{
#if SPARKLE_EDITOR_WITH_SHADER_COMPILATION
	return RunEditorApplication(SparkleLauncher::RecompileEditorShaders);
#else
	return RunEditorApplication();
#endif
}
