#pragma once

namespace qui {

struct MouseEvent {
  glm::vec2 point;
};

template <typename TEvent>
using EventHandler = std::function<void(const TEvent &)>;

} // namespace qui