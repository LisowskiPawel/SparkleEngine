#pragma once

#include "Events/EventHandle.h"
#include "Input/Dispatch/InputLayer.h"
#include "Input/Dispatch/DispatchMode.h"
#include "Input/Events/KeyboardEvent.h"
#include "Input/Events/MouseButtonEvent.h"
#include "Input/Events/MouseMoveEvent.h"
#include "Input/Events/MouseWheelEvent.h"

#include <cstddef>
#include <array>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

class InputEventDispatcher final
{
public:
	static constexpr std::size_t LayerCount = static_cast<std::size_t>(InputLayer::Count);
	using LayerAvailability = std::array<bool, LayerCount>;

	template <typename TEvent> EventHandle Subscribe(std::function<void(const TEvent&)> callback, InputLayer layer, DispatchMode mode)
	{
		if (!callback || static_cast<std::size_t>(layer) >= LayerCount)
		{
			return {};
		}

		EventHandle handle{m_nextCallbackId++};
		GetCallbacks<TEvent>().push_back({std::make_shared<Callback<TEvent>>(std::move(callback)), handle, layer, mode});
		return handle;
	}

	void Unsubscribe(EventHandle handle);

	template <typename TEvent> void DispatchImmediate(const TEvent& event, InputLayer targetLayer, const LayerAvailability& layerAvailability)
	{
		DispatchToCallbacks(event, DispatchMode::Immediate, targetLayer, layerAvailability);
	}

	template <typename TEvent> void QueueDeferred(const TEvent& event, InputLayer targetLayer, const LayerAvailability& layerAvailability)
	{
		for (const auto& entry : GetCallbacks<TEvent>())
		{
			const bool bDeferred = entry.Function && entry.Mode == DispatchMode::Deferred;
			const bool bEligible = bDeferred && ShouldDispatchToLayer(entry.Layer, targetLayer, layerAvailability);
			if (bEligible)
			{
				m_deferredEvents.push_back({event, targetLayer});
				return;
			}
		}
	}

	// Cancellation reaches both delivery modes on the affected layer, even after disablement.
	template <typename TEvent> void DispatchCancellation(const TEvent& event, InputLayer targetLayer)
	{
		const auto callbacks = SnapshotCallbacks<TEvent>([targetLayer](const auto& entry) { return entry.Layer == targetLayer; });
		for (const auto& invocation : callbacks)
		{
			(*invocation.first)(event);
		}
	}

	void ProcessDeferredEvents(const LayerAvailability& layerAvailability);
	void ClearDeferredEvents();
	void CancelAllEvents();
	void CancelLayerEvents(InputLayer layer);
	void CancelPointerEvents();

private:
	template <typename TEvent> using Callback = std::function<void(const TEvent&)>;

	template <typename TEvent> struct CallbackEntry final
	{
		std::shared_ptr<Callback<TEvent>> Function;
		EventHandle Handle;
		InputLayer Layer = InputLayer::Gameplay;
		DispatchMode Mode = DispatchMode::Immediate;
	};

	struct RoutedInputEvent final
	{
		std::variant<KeyboardEvent, MouseButtonEvent, MouseMoveEvent, MouseWheelEvent> Event;
		InputLayer TargetLayer = InputLayer::System;
	};

	using CallbackTuple = std::
	    tuple<std::vector<CallbackEntry<KeyboardEvent>>, std::vector<CallbackEntry<MouseButtonEvent>>, std::vector<CallbackEntry<MouseMoveEvent>>, std::vector<CallbackEntry<MouseWheelEvent>>>;

	static bool ShouldDispatchToLayer(InputLayer registeredLayer, InputLayer targetLayer, const LayerAvailability& layerAvailability) noexcept;

	template <typename TEvent> std::vector<CallbackEntry<TEvent>>& GetCallbacks() { return std::get<std::vector<CallbackEntry<TEvent>>>(m_callbacks); }

	template <typename TEvent, typename TPredicate> auto SnapshotCallbacks(TPredicate&& select)
	{
		// Invocation handles keep the registered callable alive without copying its mutable target.
		std::vector<std::pair<std::shared_ptr<Callback<TEvent>>, InputLayer>> callbacks;
		for (const auto& entry : GetCallbacks<TEvent>())
		{
			if (entry.Function && select(entry))
			{
				callbacks.emplace_back(entry.Function, entry.Layer);
			}
		}
		return callbacks;
	}

	template <typename TEvent> void DispatchToCallbacks(const TEvent& event, DispatchMode mode, InputLayer targetLayer, const LayerAvailability& layerAvailability)
	{
		const auto layerEpochs = m_layerEpochs;
		const auto pointerEpoch = m_pointerEpoch;
		const auto callbacks = SnapshotCallbacks<TEvent>([&](const auto& entry) { return entry.Mode == mode && ShouldDispatchToLayer(entry.Layer, targetLayer, layerAvailability); });
		for (const auto& [callback, layer] : callbacks)
		{
			const auto index = static_cast<std::size_t>(layer);
			const bool bLayerCurrent = layerEpochs[index] == m_layerEpochs[index];
			const bool bPointerCurrent = std::is_same_v<TEvent, KeyboardEvent> || pointerEpoch == m_pointerEpoch;
			if (bLayerCurrent && bPointerCurrent)
			{
				(*callback)(event);
			}
		}
	}

	CallbackTuple m_callbacks;
	std::deque<RoutedInputEvent> m_deferredEvents;
	std::array<std::uint64_t, LayerCount> m_layerEpochs{};
	std::uint64_t m_pointerEpoch = 0;
	std::uint32_t m_nextCallbackId = 1;
	bool m_bIsProcessingDeferredEvents = false;
};
