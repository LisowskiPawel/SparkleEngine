#pragma once

#include "Gui/Operations/LauncherOperationRequest.h"
#include "SparkleLauncher/OperationModel.h"
#include "SparkleLauncher/LauncherProcess.h"

#include <functional>
#include <memory>
#include <string>
#include <string_view>

namespace SparkleLauncher
{
	class LauncherOperationService final
	{
	public:
		using OutputCallback = std::function<void(std::string_view)>;
		using CompletionCallback = std::function<void(OperationRecord)>;

		LauncherOperationService();
		~LauncherOperationService();

		LauncherOperationService(const LauncherOperationService&) = delete;
		LauncherOperationService& operator=(const LauncherOperationService&) = delete;

		void Launch(LauncherOperationRequest request, std::string title, OutputCallback outputCallback, CompletionCallback completionCallback);
		bool Cancel(std::string_view runId) noexcept;
		bool ObserveGpuCrash(GpuCrashIncident incident, std::function<void(std::string)> completion);

	private:
		struct Implementation;
		std::unique_ptr<Implementation> m_implementation;
	};
}
