#pragma once

namespace quoll::editor {

/**
 * @brief Camera look at component
 */
struct CameraLookAt {
  /**
   * Camera eye
   */
  glm::vec3 eye;

  /**
   * Camera center
   */
  glm::vec3 center;

  /**
   * Camera up vector
   */
  glm::vec3 up;
};

} // namespace quoll::editor
