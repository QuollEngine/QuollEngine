#pragma once

#include "quoll/qui/component/EventProcessorBackend.h"

class MockEventProcessorBackend : public qui::EventProcessorBackendInterface {
public:
  constexpr glm::vec2 getMousePosition() override { return mMousePosition; }
  constexpr glm::vec2 getMouseWheelDelta() override { return mMouseWheelDelta; }

  constexpr bool isMouseClicked() override {
    bool temp = mMouseClicked;
    mMouseClicked = false;
    return temp;
  }

  constexpr bool isMouseDown() override {
    bool temp = mMouseDown;
    mMouseDown = false;
    return temp;
  }

  constexpr bool isMouseUp() override {
    bool temp = mMouseUp;
    mMouseUp = false;
    return temp;
  }

  constexpr bool isMouseMoved() override {
    bool temp = mMouseMove;
    mMouseMove = false;
    return temp;
  }

  constexpr bool isMouseWheel() override {
    bool temp = mMouseWheel;
    mMouseWheel = false;
    return temp;
  }

public:
  glm::vec2 mMousePosition{0.0f, 0.0f};
  glm::vec2 mMouseWheelDelta{0.0f, 0.0f};

  bool mMouseClicked{false};
  bool mMouseDown{false};
  bool mMouseUp{false};
  bool mMouseMove{false};
  bool mMouseWheel{false};
};
