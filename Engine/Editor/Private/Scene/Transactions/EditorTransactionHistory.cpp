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

static constexpr std::size_t MaximumTransactions = 256;

EditorTransactionHistory::EditorTransactionHistory(SubmitFunction submit) :
    m_submit(std::move(submit))
{
}

void EditorTransactionHistory::FinishInteraction() noexcept
{
	m_bCoalescing = false;
}

void EditorTransactionHistory::DismissLastResult() noexcept
{
	m_lastResult.reset();
}

WorldEditResult EditorTransactionHistory::Submit(WorldEditPayload payload, std::uint64_t worldGeneration)
{
	WorldEditCommand command{.RequestId = m_nextRequestId++, .Payload = std::move(payload)};
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
	m_transactions.clear();
	m_position = 0;
	if (m_lastResult && m_lastResult->IsAccepted())
	{
		m_lastResult.reset();
	}
}

WorldEditResult EditorTransactionHistory::Execute(WorldEditPayload forward, WorldEditPayload inverse, std::uint64_t worldGeneration, std::string coalescingKey)
{
	InvalidateForWorldGeneration(worldGeneration);
	WorldEditResult result = Submit(forward, worldGeneration);
	m_lastResult = result;
	if (!result.IsAccepted())
	{
		return result;
	}

	const bool bInteractionContinues = m_bCoalescing && !coalescingKey.empty();
	const bool bHasPreviousInteraction = bInteractionContinues && m_position != 0;
	const bool bMatchesPreviousKey = bHasPreviousInteraction && m_transactions[m_position - 1].CoalescingKey == coalescingKey;
	const bool bCanCoalesce = bMatchesPreviousKey && AffectsSameTarget(m_transactions[m_position - 1].Forward, forward);
	m_transactions.erase(m_transactions.begin() + m_position, m_transactions.end());

	if (bCanCoalesce)
	{
		m_transactions.back().Forward = std::move(forward);
	}
	else
	{
		if (m_transactions.size() == MaximumTransactions)
		{
			m_transactions.erase(m_transactions.begin());
		}
		m_transactions.push_back({std::move(forward), std::move(inverse), std::move(coalescingKey)});
	}
	m_position = m_transactions.size();
	m_bCoalescing = !m_transactions.back().CoalescingKey.empty();
	return result;
}

WorldEditResult EditorTransactionHistory::Undo(std::uint64_t worldGeneration)
{
	FinishInteraction();
	InvalidateForWorldGeneration(worldGeneration);
	if (m_position == 0)
	{
		m_lastResult = WorldEditResult{0, WorldEditResultStatus::Rejected, "There is no editor transaction to undo."};
		return *m_lastResult;
	}
	WorldEditResult result = Submit(m_transactions[m_position - 1].Inverse, worldGeneration);
	m_lastResult = result;
	if (result.IsAccepted())
	{
		--m_position;
	}
	return result;
}

WorldEditResult EditorTransactionHistory::Redo(std::uint64_t worldGeneration)
{
	FinishInteraction();
	InvalidateForWorldGeneration(worldGeneration);
	if (m_position == m_transactions.size())
	{
		m_lastResult = WorldEditResult{0, WorldEditResultStatus::Rejected, "There is no editor transaction to redo."};
		return *m_lastResult;
	}
	WorldEditResult result = Submit(m_transactions[m_position].Forward, worldGeneration);
	m_lastResult = result;
	if (result.IsAccepted())
	{
		++m_position;
	}
	return result;
}
