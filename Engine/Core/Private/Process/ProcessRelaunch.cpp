#include "PCH.h"

#include "Core/Public/Process/ChildProcess.h"

#include "Core/Public/Paths/FileSystemPaths.h"

#include <filesystem>
#include <system_error>

#if defined(_WIN32)
  #include <boost/asio/io_context.hpp>
  #include <boost/process/v2/windows/default_launcher.hpp>
  #include <boost/process/v2/process.hpp>
#endif

namespace Process
{
	bool RelaunchCurrentProcess()
	{
#if defined(_WIN32)
		const std::filesystem::path executable = Filesystem::GetExecutablePath();
		std::error_code error;
		const std::filesystem::path workingDirectory = std::filesystem::current_path(error);
		if (executable.empty() || error)
		{
			return false;
		}

		boost::asio::io_context context;
		boost::process::v2::windows::default_launcher launcher;
		launcher.current_directory = workingDirectory;
		boost::system::error_code launchError;
		auto child = launcher(context, launchError, executable, GetCommandLineW());
		if (launchError)
		{
			return false;
		}
		child.detach();
		return true;
#else
		return false;
#endif
	}
}
