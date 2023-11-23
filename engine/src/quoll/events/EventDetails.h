#pragma once

#include "quoll/window/MouseEvent.h"
#include "quoll/window/KeyboardEvent.h"

namespace quoll::event_detail {

/**
 * @brief Type "function" to find the correct event object
 *
 * This structure must always be specialized
 * for every new event type
 *
 * @tparam TEventType Event type
 */
template <class TEventType> struct GetEventData {};

/**
 * @brief Structure for mouse button event
 */
template <> struct GetEventData<MouseButtonEvent> {
  /**
   * Mouse button event data
   */
  using Data = MouseButtonEventObject;
};

/**
 * @brief Structure for mouse cursor event
 */
template <> struct GetEventData<MouseCursorEvent> {
  /**
   * Mouse cursor event data
   */
  using Data = MouseCursorEventObject;
};

/**
 * @brief Structure for mouse scroll event
 */
template <> struct GetEventData<MouseScrollEvent> {
  /**
   * Mouse scroll event data
   */
  using Data = MouseScrollEventObject;
};

/**
 * @brief Structure for keyboard event
 */
template <> struct GetEventData<KeyboardEvent> {
  /**
   * Keyboard event data
   */
  using Data = KeyboardEventObject;
};

} // namespace quoll::event_detail
