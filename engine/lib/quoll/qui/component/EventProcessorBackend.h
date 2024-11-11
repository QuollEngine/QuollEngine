#pragma once

namespace qui {

class EventProcessorBackendInterface {
public:
  virtual ~EventProcessorBackendInterface() = default;

  virtual glm::vec2 getMousePosition() = 0;
  virtual glm::vec2 getMouseWheelDelta() = 0;

  virtual bool isMouseClicked() = 0;
  virtual bool isMouseDown() = 0;
  virtual bool isMouseUp() = 0;
  virtual bool isMouseMoved() = 0;
  virtual bool isMouseWheel() = 0;
};

template <typename T>
concept EventProcessorBackend =
    std::is_base_of_v<EventProcessorBackendInterface, T>;

} // namespace qui
