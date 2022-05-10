#pragma once

#include "liquid/asset/Asset.h"

namespace liquid {

/**
 * @brief Animator component
 */
struct AnimatorComponent {
  /**
   * Current animation index
   */
  uint32_t currentAnimation = 0;

  /**
   * Loop animation
   */
  bool loop = false;

  /**
   * Normalized time
   *
   * Range: [0.0, 1.0];
   */
  float normalizedTime = 0.0f;

  /**
   * Animation is playing
   */
  bool playing = false;

  /**
   * List of animation handles
   */
  std::vector<AnimationAssetHandle> animations;
};

} // namespace liquid
