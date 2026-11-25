#pragma once

#include "World/WorldEditCommand.h"

#include <cstdint>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <vector>

class EditorTransactionHistory final
{
public:
	using SubmitFunction = std::function<WorldEditResult(WorldEditCommand, std::uint64_t)>;

	explicit EditorTransactionHistory(SubmitFunction submit);

	WorldEditResult Execute(WorldEditPayload forward, WorldEditPayload inverse, std::uint64_t worldGeneration, std::string coalescingKey = {});
	WorldEditResult Undo(std::uint64_t worldGeneration);
	WorldEditResult Redo(std::uint64_t worldGeneration);
	void InvalidateForWorldGeneration(std::uint64_t worldGeneration) noexcept;

	void FinishInteraction() noexcept;

	const std::optional<WorldEditResult>& GetLastResult() const noexcept { return m_lastResult; }

	void DismissLastResult() noexcept;

private:
	struct Transaction final
	{
		WorldEditPayload Forward;
		WorldEditPayload Inverse;
		std::string CoalescingKey;
	};

	WorldEditResult Submit(WorldEditPayload payload, std::uint64_t worldGeneration);
	SubmitFunction m_submit;
	std::vector<Transaction> m_transactions;
	std::size_t m_position = 0;
	std::optional<WorldEditResult> m_lastResult;
	std::uint64_t m_nextRequestId = 1;
	std::uint64_t m_worldGeneration = 0;
	bool m_bCoalescing = false;
};
