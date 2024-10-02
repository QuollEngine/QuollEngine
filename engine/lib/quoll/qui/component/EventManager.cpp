#include "quoll/core/Base.h"
#include "EventManager.h"

namespace qui {

EventHolder::~EventHolder() {
  for (auto handle : mMouseClickHandles) {
    mManager->mMouseClickHandlers.erase(handle);
  }

  for (auto handle : mMouseDownHandles) {
    mManager->mMouseDownHandlers.erase(handle);
  }

  for (auto handle : mMouseUpHandles) {
    mManager->mMouseUpHandlers.erase(handle);
  }

  for (auto handle : mMouseMoveHandles) {
    mManager->mMouseMoveHandlers.erase(handle);
  }
}

void EventHolder::registerMouseClickHandler(
    EventHandler<MouseEvent> &&handler) {
  auto handle = mManager->mMouseClickHandlers.insert(std::move(handler));
  mMouseClickHandles.push_back(handle);
}

void EventHolder::registerMouseDownHandler(EventHandler<MouseEvent> &&handler) {
  auto handle = mManager->mMouseDownHandlers.insert(std::move(handler));
  mMouseDownHandles.push_back(handle);
}

void EventHolder::registerMouseUpHandler(EventHandler<MouseEvent> &&handler) {
  auto handle = mManager->mMouseUpHandlers.insert(std::move(handler));
  mMouseUpHandles.push_back(handle);
}

void EventHolder::registerMouseMoveHandler(EventHandler<MouseEvent> &&handler) {
  auto handle = mManager->mMouseMoveHandlers.insert(std::move(handler));
  mMouseMoveHandles.push_back(handle);
}

} // namespace qui
