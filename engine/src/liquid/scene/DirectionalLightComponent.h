#pragma once

namespace liquid {

/**
 * @brief Directional light component
 */
struct DirectionalLightComponent {
  /**
   * Light color
   */
  glm::vec4 color{1.0f};

  /**
   * Light intensity
   */
  float intensity = 1.0f;

  /**
   * Light direction
   */
  glm::vec3 direction{0.0f};
};

} // namespace liquid
