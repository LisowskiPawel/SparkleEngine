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

	template <typename TEvent> EventHandle Subscribe(std::function<void(const TEvent&)> callback, InputLayer layer, DispatchMode mode)
	{
		if (!callback || static_cast<std::size_t>(layer) >= LayerCount)
		{
			return {};
		}

		const EventHandle handle{m_nextCallbackId++};
		auto& callbacks = GetCallbacks<TEvent>();
		auto function = std::make_shared<Callback<TEvent>>(std::move(callback));
		callbacks.push_back({std::move(function), handle, layer, mode});
		return handle;
	}

	void Unsubscribe(EventHandle handle);

	template <typename TEvent> void DispatchImmediate(const TEvent& event, InputLayer targetLayer) { DispatchToCallbacks(event, DispatchMode::Immediate, targetLayer); }

	template <typename TEvent> void QueueDeferred(const TEvent& event, InputLayer targetLayer)
	{
		for (const auto& entry : GetCallbacks<TEvent>())
		{
			if (entry.MatchesRoute(DispatchMode::Deferred, targetLayer))
			{
				m_deferredEvents.push_back({event, targetLayer});
				return;
			}
		}
	}

	// Cancellation reaches both delivery modes on the affected layer.
	template <typename TEvent> void DispatchCancellation(const TEvent& event, InputLayer targetLayer)
	{
		const auto callbacks = SnapshotCallbacks<TEvent>([targetLayer](const auto& entry) { return entry.Layer == targetLayer; });
		for (const auto& invocation : callbacks)
		{
			(*invocation.Function)(event);
		}
	}

	void ProcessDeferredEvents();
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

		bool MatchesRoute(DispatchMode mode, InputLayer targetLayer) const noexcept
		{
			const bool bLayerMatches = Layer == InputLayer::System || Layer == targetLayer;
			return Mode == mode && bLayerMatches;
		}
	};

	template <typename TEvent> using CallbackList = std::vector<CallbackEntry<TEvent>>;

	template <typename TEvent> struct CallbackInvocation final
	{
		std::shared_ptr<Callback<TEvent>> Function;
		InputLayer Layer;
	};

	struct RoutedInputEvent final
	{
		std::variant<KeyboardEvent, MouseButtonEvent, MouseMoveEvent, MouseWheelEvent> Event;
		InputLayer TargetLayer = InputLayer::System;
	};

	using CallbackLists = std::tuple<CallbackList<KeyboardEvent>, CallbackList<MouseButtonEvent>, CallbackList<MouseMoveEvent>, CallbackList<MouseWheelEvent>>;

	template <typename TEvent> CallbackList<TEvent>& GetCallbacks() { return std::get<CallbackList<TEvent>>(m_callbacks); }

	template <typename TEvent, typename TPredicate> auto SnapshotCallbacks(TPredicate&& select)
	{
		// Invocation handles keep the registered callable alive without copying its mutable target.
		std::vector<CallbackInvocation<TEvent>> callbacks;
		for (const auto& entry : GetCallbacks<TEvent>())
		{
			if (select(entry))
			{
				callbacks.push_back({.Function = entry.Function, .Layer = entry.Layer});
			}
		}
		return callbacks;
	}

	template <typename TEvent> void DispatchToCallbacks(const TEvent& event, DispatchMode mode, InputLayer targetLayer)
	{
		const auto layerEpochs = m_layerEpochs;
		const auto pointerEpoch = m_pointerEpoch;
		const auto matchesRoute = [mode, targetLayer](const auto& entry)
		{
			return entry.MatchesRoute(mode, targetLayer);
		};
		const auto callbacks = SnapshotCallbacks<TEvent>(matchesRoute);
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

	CallbackLists m_callbacks;
	std::deque<RoutedInputEvent> m_deferredEvents;
	std::array<std::uint64_t, LayerCount> m_layerEpochs{};
	std::uint64_t m_pointerEpoch = 0;
	std::uint32_t m_nextCallbackId = 1;
	bool m_bIsProcessingDeferredEvents = false;
};
