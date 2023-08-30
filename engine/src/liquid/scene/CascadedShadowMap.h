#pragma once

namespace quoll {

/**
 * @brief Cascaded shadow map component
 */
struct CascadedShadowMap {
  /**
   * @brief Maximum number of shadow cascades
   */
  static constexpr uint32_t MaxCascades = 6;

  /**
   * Split lambda for calculating splits
   *
   * Used to calculate split distances by combining
   * logarithmic and uniform splitting
   *
   * log_i = near * (far/near)^(i / size)
   * uniform_i = near + (far - near) * (1 / size)
   * distance_i = lambda * log_i + (1 - lambda) * uniform_i
   */
  float splitLambda = 0.8f;

  /**
   * Use soft shadows
   */
  bool softShadows = true;

  /**
   * Number of cascades
   */
  uint32_t numCascades = 4;
};

} // namespace quoll
