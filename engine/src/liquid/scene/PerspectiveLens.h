#pragma once

namespace liquid {

/**
 * @brief Perspective lens component
 */
struct PerspectiveLens {
  /**
   * Field of view
   */
  float fovY = 80.0f;

  /**
   * Near plane
   */
  float near = 0.001f;

  /**
   * Far plane
   */
  float far = 1000.0f;

  /**
   * Aspect ratio
   */
  float aspectRatio = 1.0f;
};

} // namespace liquid
