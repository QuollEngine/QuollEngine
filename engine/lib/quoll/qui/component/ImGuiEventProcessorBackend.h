#pragma once

#include "EventProcessorBackend.h"

namespace qui {

class ImGuiEventProcessorBackend : public EventProcessorBackendInterface {
public:
  glm::vec2 getMousePosition() override;
  glm::vec2 getMouseWheelDelta() override;

  bool isMouseClicked() override;
  bool isMouseDown() override;
  bool isMouseUp() override;
  bool isMouseMoved() override;
  bool isMouseWheel() override;

private:
  glm::vec2 mPreviousMousePosition{std::numeric_limits<f32>::infinity(),
                                   std::numeric_limits<f32>::infinity()};
};

} // namespace qui
