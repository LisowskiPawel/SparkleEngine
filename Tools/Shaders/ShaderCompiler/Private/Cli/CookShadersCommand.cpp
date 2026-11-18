#include "PCH.h"
#include <nlohmann/json.hpp>
#include "Core/Public/Strings/StringUtils.h"

#include "Cli/CookShadersCommand.h"

#include "Analysis/CookedShaderStatsPass.h"
#include "RHI/Public/Shaders/ShaderTarget.h"
#include "Constants/ShaderCompilerConstants.h"
#include "Cooking/GlobalShaderCooker.h"
#include "Core/Public/Diagnostics/Error.h"
#include "ToolConsole.h"
#include "ToolWorkProgressWriter.h"

#include <iostream>
#include <exception>
#include <filesystem>
#include <ostream>
#include <span>
#include <string>

static void PrintSummary(const ShaderCookResult& result, const ShaderCookSettings& settings);
static void PrintExportCompletion(const std::filesystem::path& manifest);
static int RunAnalysisPasses(const ShaderCookResult& result, const ShaderCookSettings& settings);
static std::string FormatTargets(std::span<const ShaderTarget> targets);
static void RunCookedShaderStats(const ShaderCookResult& result);

int CookShaders(const ShaderCookSettings& settings)
{
	ShaderCookResult cookResult;
	ToolWorkProgressWriter progressOutput(std::cout);
	try
	{
		cookResult = CookGlobalShaders(settings, [&progressOutput](const ToolWorkProgressEvent& progress) { progressOutput.Report(progress); });
		PrintSummary(cookResult, settings);
		PrintExportCompletion(cookResult.output.debugManifestPath);
	}
	catch (const std::exception& error)
	{
		ToolConsole::Message(std::cerr, ToolConsoleSeverity::Error, "Shader cook failed", {ToolConsole::QuotedField("reason", error.what())});
		return kExitCodeCookFailure;
	}

	if (cookResult.output.entries.empty())
	{
		return kExitCodeNoWork;
	}
	return RunAnalysisPasses(cookResult, settings);
}

static void PrintSummary(const ShaderCookResult& result, const ShaderCookSettings& settings)
{
	ToolConsole::Summary(
	    std::cout,
	    "Cooked shader generation",
	    {ToolConsole::Field("shaderTypes", std::to_string(result.selectedShaderCount)),
	        ToolConsole::Field("compileJobs", std::to_string(result.compileJobCount)),
	        ToolConsole::Field("mapEntries", std::to_string(result.output.entries.size())),
	        ToolConsole::Field("uniqueCodeRecords", std::to_string(result.output.uniqueCodeCount)),
	        ToolConsole::QuotedField("targets", FormatTargets(settings.targets))});
}

static void PrintExportCompletion(const std::filesystem::path& manifest)
{
	if (manifest.empty())
	{
		return;
	}

	const auto manifestPath = Strings::ToNarrow(manifest.wstring());
	const auto quotedManifest = nlohmann::json(manifestPath).dump();
	std::cout << "SPARKLE_SHADER_EXPORT " << quotedManifest << std::endl;
}

static int RunAnalysisPasses(const ShaderCookResult& result, const ShaderCookSettings& settings)
{
	for (const std::string& analysisPass : settings.analysisPasses)
	{
		if (analysisPass == "cooked-shader-stats")
		{
			try
			{
				RunCookedShaderStats(result);
			}
			catch (const Diagnostics::Error& error)
			{
				ToolConsole::Message(
				    std::cerr,
				    ToolConsoleSeverity::Error,
				    "Failed to run analysis pass",
				    {ToolConsole::QuotedField("analysis", "cooked-shader-stats"), ToolConsole::QuotedField("reason", error.what())});

				return kExitCodeCookFailure;
			}
			continue;
		}

		ToolConsole::Message(std::cerr, ToolConsoleSeverity::Error, "Unknown analysis pass", {ToolConsole::QuotedField("analysis", analysisPass)});
		return kExitCodeUsage;
	}

	return kExitCodeSuccess;
}

static std::string FormatTargets(std::span<const ShaderTarget> targets)
{
	std::string result;
	for (std::size_t index = 0; index < targets.size(); ++index)
	{
		if (index > 0)
		{
			result += ',';
		}

		result += GetShaderTargetName(targets[index]);
	}

	return result;
}

static void RunCookedShaderStats(const ShaderCookResult& result)
{
	const CookedShaderStatsReport report = WriteCookedShaderStats(result.output, result.outputDirectory / "Analysis");

	ToolConsole::Message(
	    std::cout,
	    ToolConsoleSeverity::Info,
	    "Analysis pass wrote output",
	    {ToolConsole::QuotedField("analysis", "cooked-shader-stats"), ToolConsole::Field("rows", std::to_string(report.rowCount)), ToolConsole::PathField("output", report.outputPath)});
}
