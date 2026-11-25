#pragma once

namespace SparkleTools
{
	using ToolMain = int(int argc, char** argv);

	int RunToolMain(int argc, char** argv, ToolMain& runTool);
}
