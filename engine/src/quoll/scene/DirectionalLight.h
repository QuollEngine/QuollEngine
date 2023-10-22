#pragma once

namespace quoll {

/**
 * @brief Directional light component
 */
struct DirectionalLight {
  /**
   * Light color
   */
  glm::vec4 color{1.0f};

  /**
   * Light intensity
   */
  f32 intensity = 1.0f;

  /**
   * Light direction
   */
  glm::vec3 direction{0.0f};
};

} // namespace quoll
