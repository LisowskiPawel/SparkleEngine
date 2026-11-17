#include "ToolConsole.h"

#include <filesystem>
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>

ToolConsoleField ToolConsole::Field(std::string_view name, std::string_view value)
{
	return ToolConsoleField{.name = std::string(name), .value = std::string(value), .style = ToolConsoleValueStyle::Raw};
}

ToolConsoleField ToolConsole::QuotedField(std::string_view name, std::string_view value)
{
	return ToolConsoleField{.name = std::string(name), .value = std::string(value), .style = ToolConsoleValueStyle::Quoted};
}

ToolConsoleField ToolConsole::PathField(std::string_view name, const std::filesystem::path& path)
{
	return QuotedField(name, path.string());
}

void ToolConsole::Error(std::string_view message)
{
	Message(std::cerr, ToolConsoleSeverity::Error, message);
}

void ToolConsole::Message(std::ostream& output, ToolConsoleSeverity severity, std::string_view message, std::initializer_list<ToolConsoleField> fields)
{
	output << GetSeverityPrefix(severity) << ' ' << message;
	WriteFields(output, fields);
	output << '\n';
}

void ToolConsole::Summary(std::ostream& output, std::string_view title, std::initializer_list<ToolConsoleField> fields)
{
	output << GetSeverityPrefix(ToolConsoleSeverity::Info) << ' ' << title << ":\n";
	for (const ToolConsoleField& field : fields)
	{
		output << "  " << field.name << '=';
		WriteFieldValue(output, field);
		output << '\n';
	}
}

const char* ToolConsole::GetSeverityPrefix(ToolConsoleSeverity severity) noexcept
{
	switch (severity)
	{
		case ToolConsoleSeverity::Info:
			return "[LOG]";
		case ToolConsoleSeverity::Warning:
			return "[WARN]";
		case ToolConsoleSeverity::Error:
			return "[ERROR]";
		default:
			return "[LOG]";
	}
}

void ToolConsole::WriteFields(std::ostream& output, std::initializer_list<ToolConsoleField> fields)
{
	for (const ToolConsoleField& field : fields)
	{
		output << ' ' << field.name << '=';
		WriteFieldValue(output, field);
	}
}

void ToolConsole::WriteFieldValue(std::ostream& output, const ToolConsoleField& field)
{
	if (field.style == ToolConsoleValueStyle::Quoted)
	{
		output << '\'' << field.value << '\'';
		return;
	}

	output << field.value;
}
