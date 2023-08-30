#pragma once

namespace quoll {

/**
 * @brief Local transform component
 */
struct LocalTransform {
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

} // namespace quoll
