#include "PCH.h"

#include "Process/ProcessReadiness.h"

#include "Core/Public/Environment/EnvironmentVariables.h"
#include "Process/ProcessReadinessEnvironment.h"

#include <string>

void Process::SignalParentReadiness(std::string_view value) noexcept
{
#if defined(_WIN32)
	std::string expectedValue;
	std::string eventName;
	const bool bExpectedValueRead = Environment::TryGetVariable(ReadinessValueEnvironmentVariable, expectedValue);
	const bool bEventNameRead = bExpectedValueRead && Environment::TryGetVariable(ReadinessEventEnvironmentVariable, eventName);
	const bool bReadinessMatches = bEventNameRead && value == expectedValue;

	if (!bReadinessMatches)
	{
		return;
	}

	// Core's parent generates ASCII names; match the native environment reader's encoding.
	const HANDLE eventHandle = OpenEventA(EVENT_MODIFY_STATE, FALSE, eventName.c_str());
	if (eventHandle == nullptr)
	{
		return;
	}
	SetEvent(eventHandle);
	CloseHandle(eventHandle);
#else
	(void) value;
#endif
}
