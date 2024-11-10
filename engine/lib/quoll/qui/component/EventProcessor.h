#pragma once

#include "EventProcessorBackend.h"
#include "Tree.h"

namespace qui {

template <EventProcessorBackend Backend> class EventProcessor {
public:
  constexpr void processEvents(Tree &tree) {
    auto *root = tree.root.getView();
    auto *globalEvents = tree.events.get();
    auto &eventContext = tree.eventContext;

    auto pos = mBackend.getMousePosition();

    if (mBackend.isMouseDown()) {
      eventContext.hitTestResult.path.clear();

      const MouseEvent ev{pos};
      if (root->hitTest(pos, eventContext.hitTestResult) &&
          eventContext.activeHitTestResult.path.empty()) {
        for (auto *v : eventContext.hitTestResult.path) {
          v->getEventDispatcher().dispatchMouseDownEvent(ev);
        }
        eventContext.activeHitTestResult = eventContext.hitTestResult;
      }

      for (auto &handler : globalEvents->getMouseDownHandlers()) {
        handler(ev);
      }
    }

    if (mBackend.isMouseUp()) {
      const MouseEvent ev{pos};
      if (!eventContext.activeHitTestResult.path.empty()) {
        for (auto *v : eventContext.activeHitTestResult.path) {
          v->getEventDispatcher().dispatchMouseUpEvent(ev);
        }

        eventContext.activeHitTestResult.path.clear();
      }

      for (auto &handler : globalEvents->getMouseUpHandlers()) {
        handler(ev);
      }
    }

    if (mBackend.isMouseClicked()) {
      eventContext.hitTestResult.path.clear();

      const MouseEvent ev{pos};
      if (root->hitTest(pos, eventContext.hitTestResult)) {
        for (auto *v : eventContext.hitTestResult.path) {
          v->getEventDispatcher().dispatchMouseClickEvent(ev);
        }
      }

      for (auto &handler : globalEvents->getMouseClickHandlers()) {
        handler(ev);
      }
    }

    if (mBackend.isMouseMoved()) {
      eventContext.hitTestResult.path.clear();
      const MouseEvent ev{pos};
      if (root->hitTest(pos, eventContext.hitTestResult)) {
        auto &current = eventContext.hitTestResult.path;
        auto &previous = eventContext.hoveredHitTestResult.path;

        usize minSize = std::min(previous.size(), current.size());
        usize mismatchIndex = minSize;
        for (usize i = 0; i < minSize; ++i) {
          if (previous.at(i) != current.at(i)) {
            mismatchIndex = i;
            break;
          }
        }

        for (usize i = mismatchIndex; i < previous.size(); ++i) {
          previous.at(i)->getEventDispatcher().dispatchMouseExitEvent(ev);
        }

        for (usize i = mismatchIndex; i < current.size(); ++i) {
          current.at(i)->getEventDispatcher().dispatchMouseEnterEvent(ev);
        }

        for (usize i = 0; i < mismatchIndex; ++i) {
          current.at(i)->getEventDispatcher().dispatchMouseMoveEvent(ev);
        }
      } else {
        for (auto *view : eventContext.hoveredHitTestResult.path) {
          view->getEventDispatcher().dispatchMouseExitEvent(ev);
        }
      }

      eventContext.hoveredHitTestResult = eventContext.hitTestResult;

      for (auto &handler : globalEvents->getMouseMoveHandlers()) {
        handler(ev);
      }
    }

    if (mBackend.isMouseWheel()) {
      eventContext.hitTestResult.path.clear();

      const MouseWheelEvent ev{.delta = mBackend.getMouseWheelDelta()};
      if (root->hitTest(pos, eventContext.hitTestResult)) {
        for (auto *v : eventContext.hitTestResult.path) {
          v->getEventDispatcher().dispatchMouseWheelEvent(ev);
        }
      }

      for (auto &handler : globalEvents->getMouseWheelHandlers()) {
        handler(ev);
      }
    }
  }

  constexpr Backend &getBackend() { return mBackend; }

private:
  Backend mBackend;
};

} // namespace qui
