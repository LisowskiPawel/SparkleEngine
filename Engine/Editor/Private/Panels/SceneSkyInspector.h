#pragma once

#include "GameFramework/Public/World/SkyEnvironment.h"

#include <string>
#include <cstdint>
#include <optional>

class EditorTransactionHistory;

class SceneSkyInspector final
{
public:
	static void Build(const std::optional<SkyEnvironment>&, EditorTransactionHistory&, std::uint64_t, const std::string&) noexcept;
};
