#pragma once

#include "Compiler/ShaderCompileRequest.h"

#include <cstdint>
#include <string>
#include <string_view>

class ShaderSourcePreprocessor final
{
public:
	static std::string Load(std::string_view sourcePath, const ShaderCompileRequest& request, std::vector<ShaderSourceFile>* sourceFiles = nullptr);

private:
	struct PreprocessContext;

	static void CaptureSourceFile(const std::string& pathKey, const std::string& sourceText, std::vector<ShaderSourceFile>* sourceFiles);

	static void VisitFile(std::string_view filePath, const ShaderCompileRequest& request, PreprocessContext& context, std::string& outSource);
	static std::string ReadSourceText(std::string_view virtualPath, const ShaderCompileRequest& request);
	static bool ContainsPragmaOnce(std::string_view sourceText);
	static std::string MakeLinePath(std::string_view path);
	static void AppendLineDirective(std::string& outSource, std::uint32_t lineNumber, std::string_view linePath);
	static void AppendExpandedInclude(std::string_view includerPath, std::string_view includeSpec, const ShaderCompileRequest& request, PreprocessContext& context, std::string& outSource);
};
