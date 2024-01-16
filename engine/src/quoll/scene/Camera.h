#pragma once

namespace quoll {

struct Camera {
  glm::mat4 projectionMatrix{1.0};

  glm::mat4 viewMatrix{1.0};

  glm::mat4 projectionViewMatrix{1.0};

  /**
   * Exposure values for camera
   *
   * First value represents the EV100 value.
   */
  glm::vec4 exposure{1.0f, 0.0f, 0.0f, 0.0f};
};

} // namespace quoll
