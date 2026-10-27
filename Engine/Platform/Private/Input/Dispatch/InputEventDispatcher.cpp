#include "PCH.h"
#include "Input/Dispatch/InputEventDispatcher.h"

#include <algorithm>

void InputEventDispatcher::Unsubscribe(EventHandle handle)
{
	if (!handle.IsValid())
	{
		return;
	}

	auto removeByHandle = [&handle](auto& callbacks)
	{
		callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(), [&handle](const auto& entry) { return entry.Handle == handle; }), callbacks.end());
	};

	std::apply([&removeByHandle](auto&... callbacks) { (removeByHandle(callbacks), ...); }, m_callbacks);
}

void InputEventDispatcher::ClearDeferredEvents()
{
	GetDeferredQueue<KeyboardEvent>().clear();
	GetDeferredQueue<MouseButtonEvent>().clear();
	GetDeferredQueue<MouseMoveEvent>().clear();
	GetDeferredQueue<MouseWheelEvent>().clear();
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
