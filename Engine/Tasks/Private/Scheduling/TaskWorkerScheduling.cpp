#include "TaskExecutorImplementation.h"

#include "ScheduledTaskExecution.h"
#include "TaskWorkerContext.h"
#include "TaskDispatchState.h"

#include "Core/Public/Threading/ThreadOwnership.h"

#include <string>

std::size_t TaskExecutor::Implementation::LaneIndex(TaskLane lane) noexcept
{
	return static_cast<std::size_t>(lane);
}

const char* TaskExecutor::Implementation::LaneName(TaskLane lane) noexcept
{
	switch (lane)
	{
		case TaskLane::FrameCritical:
			return "FrameCritical";
		case TaskLane::Background:
			return "Background";
		case TaskLane::BlockingIo:
			return "BlockingIo";
	}
	return "Invalid";
}

void TaskExecutor::Implementation::AddWorkers(TaskLane lane, std::uint32_t count)
{
	TaskLaneState& laneState = m_lanes[LaneIndex(lane)];
	laneState.Workers.reserve(count);
	for (std::uint32_t index = 0; index < count; ++index)
	{
		auto worker = std::make_unique<TaskWorker>();
		worker->Lane = lane;
		worker->LaneWorkerIndex = index;
		laneState.Workers.push_back(worker.get());
		m_workers.push_back(std::move(worker));
	}
}

std::uint32_t TaskExecutor::Implementation::GetWorkerCount(TaskLane lane) const noexcept
{
	const std::size_t index = LaneIndex(lane);
	return index < TaskLaneCount ? static_cast<std::uint32_t>(m_lanes[index].Workers.size()) : 0;
}

void TaskExecutor::Implementation::Enqueue(ReadyTask task, TaskWorker* preferredWorker, TaskLane lane)
{
	TaskLaneState& laneState = m_lanes[LaneIndex(lane)];
	if (preferredWorker != nullptr)
	{
		std::lock_guard lock(preferredWorker->QueueMutex);
		preferredWorker->ReadyQueue.push_front(std::move(task));
	}
	else
	{
		std::lock_guard lock(laneState.InjectionMutex);
		laneState.InjectionQueue.push_back(std::move(task));
	}

	{
		std::lock_guard lock(laneState.WakeMutex);
		++laneState.WakeEpoch;
	}
	laneState.WakeCondition.notify_one();
}

bool TaskExecutor::Implementation::TryPopLocal(TaskWorker& worker, ReadyTask& task)
{
	std::lock_guard lock(worker.QueueMutex);
	if (worker.ReadyQueue.empty())
	{
		return false;
	}

	task = std::move(worker.ReadyQueue.front());
	worker.ReadyQueue.pop_front();
	return true;
}

bool TaskExecutor::Implementation::TryPopInjection(TaskLane lane, ReadyTask& task)
{
	TaskLaneState& laneState = m_lanes[LaneIndex(lane)];
	std::lock_guard lock(laneState.InjectionMutex);
	if (laneState.InjectionQueue.empty())
	{
		return false;
	}

	task = std::move(laneState.InjectionQueue.front());
	laneState.InjectionQueue.pop_front();
	return true;
}

bool TaskExecutor::Implementation::TrySteal(TaskWorker& worker, ReadyTask& task)
{
	const auto& laneWorkers = m_lanes[LaneIndex(worker.Lane)].Workers;
	for (std::uint32_t offset = 1; offset < laneWorkers.size(); ++offset)
	{
		TaskWorker& victim = *laneWorkers[(worker.LaneWorkerIndex + offset) % laneWorkers.size()];
		std::lock_guard lock(victim.QueueMutex);
		if (!victim.ReadyQueue.empty())
		{
			task = std::move(victim.ReadyQueue.back());
			victim.ReadyQueue.pop_back();
			return true;
		}
	}
	return false;
}

bool TaskExecutor::Implementation::TryTakeWork(TaskWorker& worker, ReadyTask& task)
{
	if (worker.bPreferDispatch && TryPopDispatch(worker.Lane, task))
	{
		worker.bPreferDispatch = false;
		return true;
	}
	if (TryPopLocal(worker, task) || TryPopInjection(worker.Lane, task) || TrySteal(worker, task))
	{
		worker.bPreferDispatch = true;
		return true;
	}
	return TryPopDispatch(worker.Lane, task);
}

bool TaskExecutor::Implementation::WaitForWork(TaskWorker& worker, ReadyTask& task)
{
	TaskLaneState& laneState = m_lanes[LaneIndex(worker.Lane)];
	for (;;)
	{
		std::unique_lock lock(laneState.WakeMutex);
		const std::uint64_t observedWakeEpoch = laneState.WakeEpoch;
		lock.unlock();
		if (TryTakeWork(worker, task))
		{
			return true;
		}

		lock.lock();
		laneState.WakeCondition.wait(lock, [&laneState, observedWakeEpoch] { return laneState.bWorkersStopping || laneState.WakeEpoch != observedWakeEpoch; });
		if (laneState.bWorkersStopping)
		{
			return false;
		}
	}
}

void TaskExecutor::Implementation::WorkerMain(TaskWorker& worker)
{
	TaskWorkerContext::Enter(this);
	Threading::SetCurrentThreadRole("Sparkle.Task." + std::string(LaneName(worker.Lane)) + "." + std::to_string(worker.LaneWorkerIndex));

	ReadyTask task;
	while (TryTakeWork(worker, task) || WaitForWork(worker, task))
	{
		if (task.Dispatch != nullptr)
		{
			task.Dispatch->Execute();
		}
		else
		{
			task.Execution->Execute(task.TaskIndex, worker);
		}
		// Release captures outside every scheduler lock before taking another item.
		task = {};
	}

	TaskWorkerContext::Leave();
}

void TaskExecutor::Implementation::RequestWorkerStop() noexcept
{
	for (TaskLaneState& lane : m_lanes)
	{
		{
			std::lock_guard lock(lane.WakeMutex);
			lane.bWorkersStopping = true;
			++lane.WakeEpoch;
		}
		lane.WakeCondition.notify_all();
	}
}

void TaskExecutor::Implementation::JoinWorkers() noexcept
{
	for (const auto& worker : m_workers)
	{
		if (worker->Thread.joinable())
		{
			worker->Thread.join();
		}
	}
}

void TaskExecutor::Implementation::InitializeWorkers()
{
	const std::uint32_t totalWorkers = m_config.FrameCriticalWorkerCount + m_config.BackgroundWorkerCount + m_config.BlockingIoWorkerCount;
	m_workers.reserve(totalWorkers);

	AddWorkers(TaskLane::FrameCritical, m_config.FrameCriticalWorkerCount);
	AddWorkers(TaskLane::Background, m_config.BackgroundWorkerCount);
	AddWorkers(TaskLane::BlockingIo, m_config.BlockingIoWorkerCount);
}

void TaskExecutor::Implementation::StartWorkers()
{
	try
	{
		for (const auto& worker : m_workers)
		{
			worker->Thread = std::thread([this, worker = worker.get()] { WorkerMain(*worker); });
		}
	}
	catch (...)
	{
		RequestWorkerStop();
		JoinWorkers();
		throw;
	}
}

void TaskExecutor::Implementation::EnqueueDispatch(TaskDispatch::State& dispatch) noexcept
{
	TaskLaneState& lane = m_lanes[LaneIndex(dispatch.Desc.Lane)];
	{
		std::lock_guard lock(lane.InjectionMutex);
		dispatch.ReadyNext = nullptr;
		if (lane.DispatchTail != nullptr)
		{
			lane.DispatchTail->ReadyNext = &dispatch;
		}
		else
		{
			lane.DispatchHead = &dispatch;
		}
		lane.DispatchTail = &dispatch;
	}
	{
		std::lock_guard lock(lane.WakeMutex);
		++lane.WakeEpoch;
	}
	lane.WakeCondition.notify_one();
}

bool TaskExecutor::Implementation::TryPopDispatch(TaskLane laneType, ReadyTask& task)
{
	TaskLaneState& lane = m_lanes[LaneIndex(laneType)];
	std::lock_guard lock(lane.InjectionMutex);
	auto* dispatch = lane.DispatchHead;
	if (dispatch == nullptr)
	{
		return false;
	}
	lane.DispatchHead = dispatch->ReadyNext;
	if (lane.DispatchHead == nullptr)
	{
		lane.DispatchTail = nullptr;
	}
	dispatch->ReadyNext = nullptr;
	task.Dispatch = dispatch->shared_from_this();
	return true;
}
