#pragma once

#include "quoll/core/SparseSet.h"
#include "Events.h"

namespace qui {

class EventDispatcher {
  template <typename TEvent>
  static constexpr void
  callHandlers(quoll::SparseSet<EventHandler<TEvent>> &handlers,
               const TEvent &event) {
    for (const auto &handler : handlers) {
      handler(event);
    }
  }

public:
  constexpr void registerMouseDownHandler(EventHandler<MouseEvent> &&handler) {
    mMouseDownHandlers.insert(handler);
  }

  constexpr void registerMouseUpHandler(EventHandler<MouseEvent> &&handler) {
    mMouseUpHandlers.insert(handler);
  }

  constexpr void registerMouseClickHandler(EventHandler<MouseEvent> &&handler) {
    mMouseClickHandlers.insert(handler);
  }

  constexpr void registerMouseEnterHandler(EventHandler<MouseEvent> &&handler) {
    mMouseEnterHandlers.insert(handler);
  }

  constexpr void registerMouseExitHandler(EventHandler<MouseEvent> &&handler) {
    mMouseExitHandlers.insert(handler);
  }

  constexpr void registerMouseMoveHandler(EventHandler<MouseEvent> &&handler) {
    mMouseMoveHandlers.insert(handler);
  }

  constexpr void
  registerMouseWheelHandler(EventHandler<MouseWheelEvent> &&handler) {
    mMouseWheelHandlers.insert(handler);
  }

  constexpr void dispatchMouseDownEvent(const MouseEvent &event) {
    callHandlers(mMouseDownHandlers, event);
  }

  constexpr void dispatchMouseUpEvent(const MouseEvent &event) {
    callHandlers(mMouseUpHandlers, event);
  }

  constexpr void dispatchMouseClickEvent(const MouseEvent &event) {
    callHandlers(mMouseClickHandlers, event);
  }

  constexpr void dispatchMouseEnterEvent(const MouseEvent &event) {
    callHandlers(mMouseEnterHandlers, event);
  }

  constexpr void dispatchMouseExitEvent(const MouseEvent &event) {
    callHandlers(mMouseExitHandlers, event);
  }

  constexpr void dispatchMouseMoveEvent(const MouseEvent &event) {
    callHandlers(mMouseMoveHandlers, event);
  }

  constexpr void dispatchMouseWheelEvent(const MouseWheelEvent &event) {
    callHandlers(mMouseWheelHandlers, event);
  }

private:
  quoll::SparseSet<EventHandler<MouseEvent>> mMouseDownHandlers;
  quoll::SparseSet<EventHandler<MouseEvent>> mMouseUpHandlers;
  quoll::SparseSet<EventHandler<MouseEvent>> mMouseClickHandlers;
  quoll::SparseSet<EventHandler<MouseEvent>> mMouseMoveHandlers;
  quoll::SparseSet<EventHandler<MouseEvent>> mMouseEnterHandlers;
  quoll::SparseSet<EventHandler<MouseEvent>> mMouseExitHandlers;
  quoll::SparseSet<EventHandler<MouseWheelEvent>> mMouseWheelHandlers;
};

} // namespace qui
