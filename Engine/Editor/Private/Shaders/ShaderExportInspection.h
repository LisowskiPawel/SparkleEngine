#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

class ShaderExportInspection final
{
public:
	struct Texts final
	{
		std::string Source;
		std::string Reflection;
		std::string Disassembly;
		std::string ParameterMatch;
		std::string CompileRequest;
	};

	static std::filesystem::path FindExportDirectory(std::string_view shaderName);
	void InspectExport(const std::filesystem::path& directory);
	void Invalidate() noexcept;

	bool HasExport() const noexcept { return m_texts.has_value(); }

	const Texts& GetTexts() const noexcept { return *m_texts; }

	const std::filesystem::path& GetDirectory() const noexcept { return m_directory; }

private:
	std::filesystem::path m_directory;
	std::optional<Texts> m_texts;
};
