#include "Cli/AssetCookerCli.h"

#include "ToolMain.h"

int main(int argc, char** argv)
{
	return SparkleTools::RunToolMain(argc, argv, RunAssetCooker);
}
