#pragma once

#include "quoll/qui/component/EventProcessor.h"
#include "MockEventProcessorBackend.h"

class MockEventController {
public:
  MockEventController(qui::Tree &tree) : mTree(&tree) {}

  constexpr void click(glm::vec2 position) {
    events.mMouseClicked = true;
    events.mMousePosition = position;
    eventProcessor.processEvents(*mTree);
  }

  constexpr void mouseDown(glm::vec2 position) {
    events.mMouseDown = true;
    events.mMousePosition = position;
    eventProcessor.processEvents(*mTree);
  }

  constexpr void mouseUp(glm::vec2 position) {
    events.mMouseUp = true;
    events.mMousePosition = position;
    eventProcessor.processEvents(*mTree);
  }

  constexpr void mouseMove(glm::vec2 position) {
    events.mMouseMove = true;
    events.mMousePosition = position;
    eventProcessor.processEvents(*mTree);
  }

  constexpr void mouseWheel(glm::vec2 delta) {
    events.mMouseWheel = true;
    events.mMouseWheelDelta = delta;
    eventProcessor.processEvents(*mTree);
  }

public:
  qui::EventProcessor<MockEventProcessorBackend> eventProcessor;
  MockEventProcessorBackend &events = eventProcessor.getBackend();
  qui::Tree *mTree = nullptr;
};
