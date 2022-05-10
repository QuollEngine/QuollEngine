#pragma once

namespace liquid {

/**
 * @brief World transform component
 */
struct WorldTransformComponent {
  /**
   * World transform matrix
   */
  glm::mat4 worldTransform{1.0f};
};

} // namespace liquid
