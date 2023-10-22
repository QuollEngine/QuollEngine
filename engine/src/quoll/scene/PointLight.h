#pragma once

namespace quoll {

/**
 * @brief Point light for entity
 */
struct PointLight {
  /**
   * Light color
   */
  glm::vec4 color{1.0};

  /**
   * Light intensity
   */
  f32 intensity = 1.0f;

  /**
   * Light range cutoff
   */
  f32 range = 10.0f;
};

} // namespace quoll
