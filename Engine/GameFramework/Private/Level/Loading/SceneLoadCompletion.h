#pragma once

#include "Level/Loading/SceneLoadPackage.h"

#include <cstdint>
#include <memory>
#include <string>

namespace Assets
{
	enum class SceneLoadStatus : std::uint8_t
	{
		Pending,
		Ready,
		Failed,
		Cancelled
	};

	struct SceneLoadCompletion final
	{
		std::unique_ptr<SceneLoadPackage> Package;
		std::uint64_t RequestId = 0;
		SceneLoadStatus Status = SceneLoadStatus::Pending;
		std::string Diagnostic;
	};
}
