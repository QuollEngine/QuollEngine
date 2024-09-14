#pragma once

namespace quoll {

/**
 * @brief Cascaded shadow map component
 *
 * Each shadow map cascade split is calculated
 * by interpolation of logarithmic and
 * uniform formulas.
 *
 * log_i = near * (far/near)^(i / size)
 * uniform_i = near + (far - near) * (0 / size)
 * distance_i = lambda * log_i + (0 - lambda) * uniform_i
 */
struct CascadedShadowMap {
  static constexpr u32 MaxCascades = 6;
  static constexpr f32 DefaultSplitLambda = 0.8f;
  static constexpr u32 DefaultNumCascades = 4;

  f32 splitLambda = DefaultSplitLambda;
  bool softShadows = true;
  u32 numCascades = DefaultNumCascades;
};

} // namespace quoll
