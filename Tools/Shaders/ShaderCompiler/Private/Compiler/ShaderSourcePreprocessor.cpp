#include "PCH.h"

#include "Compiler/ShaderSourcePreprocessor.h"

#include "Compiler/ShaderIncludeResolver.h"
#include "Compiler/ShaderSourceMountTable.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"

#include <format>
#include <algorithm>
#include <regex>
#include <sstream>
#include <unordered_set>

struct ShaderPreprocessContext;

static void CaptureSourceFile(const std::string& pathKey, const std::string& sourceText, std::vector<ShaderSourceFile>* sourceFiles);

static void VisitFile(std::string_view filePath, const ShaderCompileRequest& request, ShaderPreprocessContext& context, std::string& outSource);
static std::string ReadSourceText(std::string_view virtualPath, const ShaderCompileRequest& request);
static bool ContainsPragmaOnce(std::string_view sourceText);
static std::string MakeLinePath(std::string_view path);
static void AppendLineDirective(std::string& outSource, std::uint32_t lineNumber, std::string_view linePath);
static void AppendExpandedInclude(std::string_view includerPath, std::string_view includeSpec, const ShaderCompileRequest& request, ShaderPreprocessContext& context, std::string& outSource);

struct ShaderPreprocessContext final
{
	std::unordered_set<std::string> OncePathKeys;
	std::unordered_set<std::string> ActivePathKeys;
	std::vector<ShaderSourceFile>* SourceFiles = nullptr;
};

static void VisitFile(std::string_view filePath, const ShaderCompileRequest& request, ShaderPreprocessContext& context, std::string& outSource)
{
	const std::string pathKey = request.SourceMounts.get().CanonicalizeVirtualPath(filePath);

	const std::string sourceText = ReadSourceText(pathKey, request);

	if (ContainsPragmaOnce(sourceText) && !context.OncePathKeys.insert(pathKey).second)
	{
		return;
	}

	if (!context.ActivePathKeys.insert(pathKey).second)
	{
		throw Diagnostics::Error(std::format("Detected recursive shader include while preprocessing '{}'", pathKey));
	}
	CaptureSourceFile(pathKey, sourceText, context.SourceFiles);

	const std::string linePath = MakeLinePath(pathKey);
	AppendLineDirective(outSource, 1u, linePath);

	static const std::regex includeRegex(R"(^\s*#\s*include\s*["<]([^">]+)[">])", std::regex::icase);
	static const std::regex pragmaOnceRegex(R"(^\s*#\s*pragma\s+once\b)", std::regex::icase);

	std::istringstream stream(sourceText);
	std::string line;
	std::smatch match;
	std::uint32_t lineNumber = 1;
	while (std::getline(stream, line))
	{
		if (std::regex_search(line, pragmaOnceRegex))
		{
			outSource += '\n';
			++lineNumber;
			continue;
		}

		if (std::regex_search(line, match, includeRegex))
		{
			const std::string includeSpec = match[1].str();
			AppendExpandedInclude(pathKey, includeSpec, request, context, outSource);
			AppendLineDirective(outSource, lineNumber + 1u, linePath);
		}
		else
		{
			outSource += line;
			outSource += '\n';
		}

		++lineNumber;
	}

	context.ActivePathKeys.erase(pathKey);
}

static void CaptureSourceFile(const std::string& pathKey, const std::string& sourceText, std::vector<ShaderSourceFile>* sourceFiles)
{
	if (sourceFiles != nullptr)
	{
		const auto existing = std::ranges::find(*sourceFiles, pathKey, &ShaderSourceFile::VirtualPath);
		if (existing == sourceFiles->end())
		{
			sourceFiles->push_back({.VirtualPath = pathKey, .Contents = sourceText});
		}
		else if (existing->Contents != sourceText)
		{
			throw Diagnostics::Error(std::format("Shader source '{}' changed while preprocessing.", pathKey));
		}
	}
}

static std::string ReadSourceText(std::string_view virtualPath, const ShaderCompileRequest& request)
{
	const std::filesystem::path filePath = request.SourceMounts.get().ResolvePhysicalPath(virtualPath);
	std::vector<std::uint8_t> bytes;
	std::string fileError;
	if (!Files::TryReadAllBytes(filePath, bytes, fileError))
	{
		throw Diagnostics::Error(std::format("Failed to read shader source '{}' while preprocessing includes.", virtualPath));
	}

	return std::string(reinterpret_cast<const char*>(bytes.data()), bytes.size());
}

static bool ContainsPragmaOnce(std::string_view sourceText)
{
	static const std::regex pragmaOnceRegex(R"(^\s*#\s*pragma\s+once\b)", std::regex::icase);
	std::istringstream stream(std::string{sourceText});
	std::string line;
	while (std::getline(stream, line))
	{
		if (std::regex_search(line, pragmaOnceRegex))
		{
			return true;
		}
	}
	return false;
}

static std::string MakeLinePath(std::string_view path)
{
	std::string linePath(path);
	for (std::size_t index = 0; index < linePath.size(); ++index)
	{
		if (linePath[index] == '"')
		{
			linePath.insert(index, 1u, '\\');
			++index;
		}
	}
	return linePath;
}

static void AppendLineDirective(std::string& outSource, std::uint32_t lineNumber, std::string_view linePath)
{
	outSource += "#line ";
	outSource += std::to_string(lineNumber);
	outSource += " \"";
	outSource += linePath;
	outSource += "\"\n";
}

static void AppendExpandedInclude(std::string_view includerPath, std::string_view includeSpec, const ShaderCompileRequest& request, ShaderPreprocessContext& context, std::string& outSource)
{
	const auto includePath = ShaderIncludeResolver::ResolveIncludePath(includerPath, includeSpec, request);
	if (!includePath)
	{
		throw Diagnostics::Error(std::format("Failed to resolve include '{}' referenced from '{}' while preprocessing shader source", includeSpec, includerPath));
	}

	VisitFile(*includePath, request, context, outSource);
}

std::string PreprocessShaderSource(std::string_view sourcePath, const ShaderCompileRequest& request, std::vector<ShaderSourceFile>* sourceFiles)
{
	std::string sourceText;
	if (request.ParameterStruct && !request.ParameterStruct->Values.empty())
	{
		const auto& descriptor = *request.ParameterStruct;
		sourceText = "cbuffer " + descriptor.AutoParametersName + "\n{\n";
		for (const auto& value : descriptor.Values)
		{
			sourceText += value.HlslType + " " + value.Name + " : packoffset(c" + std::to_string(value.OffsetInBytes / 16u) + "." + "xyzw"[value.OffsetInBytes % 16u / 4u] + ");\n";
		}
		sourceText += "};\n";
	}
	ShaderPreprocessContext context{.SourceFiles = sourceFiles};
	VisitFile(sourcePath, request, context, sourceText);
	return sourceText;
}
