#include "quoll/core/Base.h"
#include "EventDispatcher.h"

namespace qui {

namespace {

template <typename TEvent>
void callHandlers(quoll::SparseSet<EventHandler<TEvent>> &handlers,
                  const TEvent &event) {
  for (const auto &handler : handlers) {
    handler(event);
  }
}

} // namespace

void EventDispatcher::dispatchMouseDownEvent(const MouseEvent &event) {
  callHandlers(mMouseDownHandlers, event);
}

void EventDispatcher::dispatchMouseUpEvent(const MouseEvent &event) {
  callHandlers(mMouseUpHandlers, event);
}

void EventDispatcher::dispatchMouseClickEvent(const MouseEvent &event) {
  callHandlers(mMouseClickHandlers, event);
}

void EventDispatcher::dispatchMouseEnterEvent(const MouseEvent &event) {
  callHandlers(mMouseEnterHandlers, event);
}

void EventDispatcher::dispatchMouseExitEvent(const MouseEvent &event) {
  callHandlers(mMouseExitHandlers, event);
}

void EventDispatcher::dispatchMouseMoveEvent(const MouseEvent &event) {
  callHandlers(mMouseMoveHandlers, event);
}

void EventDispatcher::dispatchMouseWheelEvent(const MouseWheelEvent &event) {
  callHandlers(mMouseWheelHandlers, event);
}

} // namespace qui
