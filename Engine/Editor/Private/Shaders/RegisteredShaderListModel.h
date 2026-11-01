#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

struct RegisteredShaderRow final
{
	std::string ShaderName;
	std::uint64_t ShaderTypeId = 0;
	std::string SourcePath;
	std::string EntryPoint;
	std::string Stage;
	std::size_t ParameterCount = 0;
	std::uint64_t RuntimeGeneration = 0;
	std::filesystem::path ExportDirectory;
};

class RegisteredShaderListModel final
{
public:
	using GenerationProvider = std::function<std::uint64_t()>;

	void SetGenerationProvider(GenerationProvider provider);
	void Refresh();

	const std::vector<RegisteredShaderRow>& GetRows() const noexcept { return m_rows; }

private:
	GenerationProvider m_generationProvider;
	std::vector<RegisteredShaderRow> m_rows;
};
