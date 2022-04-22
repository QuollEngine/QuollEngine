#pragma once

#include "liquid/physics/CollisionEvent.h"
#include "liquid/window/MouseEvent.h"
#include "liquid/window/KeyboardEvent.h"

namespace liquid::event_detail {

/**
 * @brief Type "function" to find the correct event object
 *
 * This structure must always be specialized
 * for every new event type
 *
 * @tparam TEventType Event type
 */
template <class TEventType> struct GetEventData {};

template <> struct GetEventData<CollisionEvent> {
  using Data = CollisionObject;
};

template <> struct GetEventData<MouseButtonEvent> {
  using Data = MouseButtonEventObject;
};

template <> struct GetEventData<MouseCursorEvent> {
  using Data = MouseCursorEventObject;
};

template <> struct GetEventData<MouseScrollEvent> {
  using Data = MouseScrollEventObject;
};

template <> struct GetEventData<KeyboardEvent> {
  using Data = KeyboardEventObject;
};

} // namespace liquid::event_detail
