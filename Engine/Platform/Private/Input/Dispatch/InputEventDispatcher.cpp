#include "PCH.h"
#include "Input/Dispatch/InputEventDispatcher.h"

#include <algorithm>

void InputEventDispatcher::Unsubscribe(EventHandle handle)
{
	auto removeByHandle = [handle](auto& callbacks)
	{
		std::erase_if(callbacks, [handle](const auto& entry) { return entry.Handle == handle; });
	};

	std::apply([&removeByHandle](auto&... callbacks) { (removeByHandle(callbacks), ...); }, m_callbacks);
}

void InputEventDispatcher::ProcessDeferredEvents()
{
	if (m_bIsProcessingDeferredEvents)
	{
		return;
	}

	m_bIsProcessingDeferredEvents = true;
	try
	{
		while (!m_deferredEvents.empty())
		{
			// Remove before invoking user code, which can clear, append or recursively process the queue.
			const RoutedInputEvent routedEvent = std::move(m_deferredEvents.front());
			m_deferredEvents.pop_front();
			const auto dispatchEvent = [&](const auto& event)
			{
				DispatchToCallbacks(event, DispatchMode::Deferred, routedEvent.TargetLayer);
			};
			std::visit(dispatchEvent, routedEvent.Event);
		}
	}
	catch (...)
	{
		m_bIsProcessingDeferredEvents = false;
		throw;
	}
	m_bIsProcessingDeferredEvents = false;
}

void InputEventDispatcher::ClearDeferredEvents()
{
	m_deferredEvents.clear();
}

void InputEventDispatcher::CancelAllEvents()
{
	for (auto& epoch : m_layerEpochs)
	{
		++epoch;
	}
	m_deferredEvents.clear();
}

void InputEventDispatcher::CancelLayerEvents(InputLayer layer)
{
	const auto index = static_cast<std::size_t>(layer);
	++m_layerEpochs[index];
	for (auto& event : m_deferredEvents)
	{
		if (event.TargetLayer == layer)
		{
			// Physical input remains accepted globally when a routed layer is cancelled.
			event.TargetLayer = InputLayer::System;
		}
	}
}

void InputEventDispatcher::CancelPointerEvents()
{
	++m_pointerEpoch;
	std::erase_if(m_deferredEvents, [](const RoutedInputEvent& event) { return !std::holds_alternative<KeyboardEvent>(event.Event); });
}
