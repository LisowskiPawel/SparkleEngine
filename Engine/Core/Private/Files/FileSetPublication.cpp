#include "PCH.h"

#include "Core/Public/Files/FileSetPublication.h"

#include "Files/FileSetCoordination.h"
#include "Files/FileSetReplacement.h"
#include "Core/Public/Diagnostics/Error.h"

#include <format>
#include <memory>
#include <vector>

namespace Files
{
	struct FileSetPublisher::Implementation final
	{
		explicit Implementation(std::span<const std::filesystem::path> roots) :
		    Roots(PublicationRoots(roots))
		{
			for (const auto& root : Roots)
			{
				Locks.push_back(std::make_unique<FileSetLock>(root, true));
			}
			RequireCompletePublications(Roots);
		}

		bool Publish(std::span<const FilePublication> files, std::string& outErrorMessage)
		{
			std::vector<FileSetReplacement> replacements;
			bool bStarted = false;
			bool bCommitted = false;
			try
			{
				RequireCompletePublications(Roots);
				replacements = PlanFileSetReplacements(files, Roots);
				bStarted = !files.empty();
				if (bStarted)
				{
					MarkPublicationsIncomplete(Roots);
				}
				ReplaceFileSet(replacements);

				bCommitted = true;
				DiscardFileSetBackups(replacements);
				ClearPublicationMarkers(Roots);
				outErrorMessage.clear();
				return true;
			}
			catch (const Diagnostics::Error& error)
			{
				outErrorMessage = error.what();
				if (bStarted && !bCommitted)
				{
					try
					{
						RollbackFileSet(replacements);
						ClearPublicationMarkers(Roots);
						return false;
					}
					catch (const Diagnostics::Error& rollback)
					{
						outErrorMessage += std::format(" Rollback failed: {}", rollback.what());
					}
				}
				if (bStarted)
				{
					outErrorMessage += " Clean generated output and cook again.";
				}
				return false;
			}
		}

		std::vector<std::filesystem::path> Roots;
		std::vector<std::unique_ptr<FileSetLock>> Locks;
	};

	FileSetPublisher::FileSetPublisher(std::span<const std::filesystem::path> roots) :
	    m_implementation(std::make_unique<Implementation>(roots))
	{
	}

	FileSetPublisher::~FileSetPublisher() = default;

	bool FileSetPublisher::Publish(std::span<const FilePublication> files, std::string& outErrorMessage)
	{
		return m_implementation->Publish(files, outErrorMessage);
	}
}
