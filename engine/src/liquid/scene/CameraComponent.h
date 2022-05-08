#pragma once

namespace liquid {

struct CameraComponent {
  glm::mat4 projectionMatrix{1.0};
  glm::mat4 viewMatrix{1.0};
  glm::mat4 projectionViewMatrix{1.0};
};

} // namespace liquid
