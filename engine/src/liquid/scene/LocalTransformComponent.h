#pragma once

namespace liquid {

/**
 * @brief Local transform component
 */
struct LocalTransformComponent {
  /**
   * Local position
   */
  glm::vec3 localPosition{0.0f};

  /**
   * Local rotation
   */
  glm::quat localRotation{1.0f, 0.0f, 0.0f, 0.0f};

  /**
   * Local scale
   */
  glm::vec3 localScale{1.0f};
};

} // namespace liquid
