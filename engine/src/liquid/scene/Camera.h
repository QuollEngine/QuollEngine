#pragma once

namespace liquid {

/**
 * @brief Camera component
 */
struct Camera {
  /**
   * Camera projection matrix
   */
  glm::mat4 projectionMatrix{1.0};

  /**
   * Camera view matrix
   */
  glm::mat4 viewMatrix{1.0};

  /**
   * Camera projection view matrix
   */
  glm::mat4 projectionViewMatrix{1.0};
};

} // namespace liquid
