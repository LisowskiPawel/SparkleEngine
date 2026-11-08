#pragma once

struct ShaderCookSettings;

namespace CLI
{
	class App;
}

void ConfigureShaderCookOptions(CLI::App& cook, ShaderCookSettings& settings);
void ValidateShaderCookSelection(const ShaderCookSettings& settings);
