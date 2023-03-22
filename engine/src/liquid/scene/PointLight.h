#pragma once

namespace liquid {

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
  float intensity = 1.0f;

  /**
   * Light range cutoff
   */
  float range = 10.0f;
};

} // namespace liquid
