#pragma once

#include "quoll/qui/component/BuildContext.h"
#include "quoll-tests/Testing.h"

class QuiComponentTest : public ::testing::Test {
public:
  void dispatchMouseWheelEvent(qui::MouseWheelEvent ev) {
    for (auto &handler : eventManager.getMouseWheelHandlers()) {
      handler(ev);
    }
  }

  void dispatchMouseMoveEvent(qui::MouseEvent ev) {
    for (auto &handler : eventManager.getMouseMoveHandlers()) {
      handler(ev);
    }
  }

  void dispatchMouseDownEvent(qui::MouseEvent ev) {
    for (auto &handler : eventManager.getMouseDownHandlers()) {
      handler(ev);
    }
  }

  void dispatchMouseUpEvent(qui::MouseEvent ev) {
    for (auto &handler : eventManager.getMouseUpHandlers()) {
      handler(ev);
    }
  }

public:
  qui::EventManager eventManager;
  qui::BuildContext buildContext{&eventManager};
};
