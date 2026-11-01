#pragma once

#include "Core/Public/CoreAPI.h"

#include <filesystem>
#include <memory>
#include <span>
#include <string>

namespace Files
{
	struct FilePublication final
	{
		std::filesystem::path StagedPath;
		std::filesystem::path PublishedPath;
	};

	class SPARKLE_CORE_API FileSetReadScope final
	{
	public:
		explicit FileSetReadScope(const std::filesystem::path& root);
		~FileSetReadScope();

		FileSetReadScope(const FileSetReadScope&) = delete;
		FileSetReadScope& operator=(const FileSetReadScope&) = delete;

		bool Contains(const std::filesystem::path& path) const;

	private:
		struct Implementation;
		std::unique_ptr<Implementation> m_implementation;
	};

	class SPARKLE_CORE_API FileSetPublisher final
	{
	public:
		// Acquire before staging. Interrupted output requires clean/regenerate; no journal replay.
		explicit FileSetPublisher(std::span<const std::filesystem::path> roots);
		~FileSetPublisher();

		FileSetPublisher(const FileSetPublisher&) = delete;
		FileSetPublisher& operator=(const FileSetPublisher&) = delete;

		bool Publish(std::span<const FilePublication> files, std::string& outErrorMessage);

	private:
		struct Implementation;
		std::unique_ptr<Implementation> m_implementation;
	};
}
