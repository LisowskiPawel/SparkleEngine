#include "PCH.h"
#include "Scene/Transactions/EditorTransactionHistory.h"

#include <utility>
#include <type_traits>

static bool AffectsSameTarget(const WorldEditPayload& left, const WorldEditPayload& right)
{
	if (left.index() != right.index())
	{
		return false;
	}

	return std::visit(
	    [&right](const auto& command)
	    {
		    using Command = std::remove_cvref_t<decltype(command)>;
		    if constexpr (requires { command.Entity; })
		    {
			    return command.Entity == std::get<Command>(right).Entity;
		    }
		    return true;
	    },
	    left);
}

class EditorTransactionHistoryLimits final
{
public:
	static constexpr std::size_t MaximumTransactions = 256;
};

WorldEditResult EditorTransactionHistory::Submit(WorldEditCommand command, std::uint64_t worldGeneration)
{
	command.RequestId = m_nextRequestId++;
	if (!m_submit)
	{
		return {command.RequestId, WorldEditResultStatus::Rejected, "The editor command boundary is unavailable."};
	}
	return m_submit(std::move(command), worldGeneration);
}

void EditorTransactionHistory::InvalidateForWorldGeneration(std::uint64_t worldGeneration) noexcept
{
	if (m_worldGeneration == worldGeneration)
	{
		return;
	}
	m_worldGeneration = worldGeneration;
	FinishInteraction();
	m_undo.clear();
	m_redo.clear();
	if (m_lastResult && m_lastResult->IsAccepted())
	{
		m_lastResult.reset();
	}
}

WorldEditResult EditorTransactionHistory::Execute(WorldEditCommand forward, WorldEditCommand inverse, std::uint64_t worldGeneration, std::string coalescingKey)
{
	InvalidateForWorldGeneration(worldGeneration);
	WorldEditResult result = Submit(forward, worldGeneration);
	m_lastResult = result;
	if (!result.IsAccepted())
	{
		return result;
	}

	const bool bCoalesceTransactionEmpty = m_bCoalescing && !coalescingKey.empty();
	const bool bCanCoalesceTransactionUndoEmpty = bCoalesceTransactionEmpty && (!m_undo.empty());
	const bool bCanCoalesceTransactionBack = bCanCoalesceTransactionUndoEmpty && (m_undo.back().CoalescingKey == coalescingKey);
	const bool bCanCoalesceTransaction = bCanCoalesceTransactionBack && (AffectsSameTarget(m_undo.back().Forward.Payload, forward.Payload));

	if (bCanCoalesceTransaction)
	{
		m_undo.back().Forward = std::move(forward);
	}
	else
	{
		if (m_undo.size() == EditorTransactionHistoryLimits::MaximumTransactions)
		{
			m_undo.erase(m_undo.begin());
		}
		m_undo.push_back({std::move(forward), std::move(inverse), std::move(coalescingKey)});
	}
	m_redo.clear();
	m_bCoalescing = !m_undo.back().CoalescingKey.empty();
	return result;
}

WorldEditResult EditorTransactionHistory::Undo(std::uint64_t worldGeneration)
{
	FinishInteraction();
	InvalidateForWorldGeneration(worldGeneration);
	if (m_undo.empty())
	{
		m_lastResult = WorldEditResult{0, WorldEditResultStatus::Rejected, "There is no editor transaction to undo."};
		return *m_lastResult;
	}
	Transaction transaction = m_undo.back();
	WorldEditResult result = Submit(transaction.Inverse, worldGeneration);
	m_lastResult = result;
	if (result.IsAccepted())
	{
		m_undo.pop_back();
		m_redo.push_back(std::move(transaction));
	}
	return result;
}

WorldEditResult EditorTransactionHistory::Redo(std::uint64_t worldGeneration)
{
	FinishInteraction();
	InvalidateForWorldGeneration(worldGeneration);
	if (m_redo.empty())
	{
		m_lastResult = WorldEditResult{0, WorldEditResultStatus::Rejected, "There is no editor transaction to redo."};
		return *m_lastResult;
	}
	Transaction transaction = m_redo.back();
	WorldEditResult result = Submit(transaction.Forward, worldGeneration);
	m_lastResult = result;
	if (result.IsAccepted())
	{
		m_redo.pop_back();
		m_undo.push_back(std::move(transaction));
	}
	return result;
}
