#pragma once

#include <filesystem>
#include <string>

struct RegisteredShaderRow;

class ShaderArtifactInspection final
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

	void Select(const RegisteredShaderRow* selectedRow);
	void Invalidate() noexcept;

	const Texts& GetTexts() const noexcept { return m_texts; }

	const std::filesystem::path& GetDirectory() const noexcept { return m_directory; }

private:
	std::string m_loadedShaderId;
	std::filesystem::path m_directory;
	Texts m_texts;
};
