#include "PCH.h"
#include "Input/Dispatch/InputEventDispatcher.h"

#include <algorithm>

void InputEventDispatcher::Unsubscribe(EventHandle handle)
{
	if (!handle.IsValid())
	{
		return;
	}

	auto removeByHandle = [handle](auto& callbacks)
	{
		std::erase_if(callbacks, [handle](const auto& entry) { return entry.Handle == handle; });
	};

	std::apply([&removeByHandle](auto&... callbacks) { (removeByHandle(callbacks), ...); }, m_callbacks);
}

void InputEventDispatcher::ProcessDeferredEvents(const LayerAvailability& layerAvailability)
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
			std::visit([&](const auto& event) { DispatchToCallbacks(event, DispatchMode::Deferred, routedEvent.TargetLayer, layerAvailability); }, routedEvent.Event);
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
	if (index >= LayerCount)
	{
		return;
	}

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

bool InputEventDispatcher::ShouldDispatchToLayer(InputLayer registeredLayer, InputLayer targetLayer, const LayerAvailability& layerAvailability) noexcept
{
	if (registeredLayer == InputLayer::System)
	{
		return true;
	}
	if (registeredLayer != targetLayer)
	{
		return false;
	}

	const auto index = static_cast<std::size_t>(registeredLayer);
	return index < layerAvailability.size() && layerAvailability[index];
}
