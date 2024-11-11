#pragma once

#include "quoll/core/SparseSet.h"
#include "Events.h"

namespace qui {

class EventDispatcher {
public:
  inline void registerMouseDownHandler(EventHandler<MouseEvent> &&handler) {
    mMouseDownHandlers.insert(handler);
  }
  inline void registerMouseUpHandler(EventHandler<MouseEvent> &&handler) {
    mMouseUpHandlers.insert(handler);
  }
  inline void registerMouseClickHandler(EventHandler<MouseEvent> &&handler) {
    mMouseClickHandlers.insert(handler);
  }
  inline void registerMouseEnterHandler(EventHandler<MouseEvent> &&handler) {
    mMouseEnterHandlers.insert(handler);
  }
  inline void registerMouseExitHandler(EventHandler<MouseEvent> &&handler) {
    mMouseExitHandlers.insert(handler);
  }

  inline void registerMouseMoveHandler(EventHandler<MouseEvent> &&handler) {
    mMouseMoveHandlers.insert(handler);
  }
  inline void
  registerMouseWheelHandler(EventHandler<MouseWheelEvent> &&handler) {
    mMouseWheelHandlers.insert(handler);
  }

  void dispatchMouseDownEvent(const MouseEvent &event);
  void dispatchMouseUpEvent(const MouseEvent &event);
  void dispatchMouseClickEvent(const MouseEvent &event);
  void dispatchMouseEnterEvent(const MouseEvent &event);
  void dispatchMouseExitEvent(const MouseEvent &event);
  void dispatchMouseMoveEvent(const MouseEvent &event);
  void dispatchMouseWheelEvent(const MouseWheelEvent &event);

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
