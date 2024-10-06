#pragma once

namespace qui {

struct MouseEvent {
  glm::vec2 point;
};

struct MouseWheelEvent {
  glm::vec2 delta;
};

template <typename TEvent>
using EventHandler = std::function<void(const TEvent &)>;

} // namespace qui
