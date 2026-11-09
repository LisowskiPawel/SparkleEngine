#pragma once

struct ShaderCookSettings;

namespace CLI
{
	class App;
}

void ConfigureShaderCookOptions(CLI::App& cook, ShaderCookSettings& settings);
