#pragma once

#include "quoll/asset/Asset.h"

namespace quoll {

/**
 * @brief Animator component
 */
struct Animator {
  /**
   * Animator asset handle
   */
  AnimatorAssetHandle asset = AnimatorAssetHandle::Null;

  /**
   * Current animator state
   */
  usize currentState = std::numeric_limits<usize>::max();

  /**
   * Normalized time
   *
   * Range: [0.0, 1.0];
   */
  f32 normalizedTime = 0.0f;

  /**
   * Animation is playing
   */
  bool playing = true;
};

} // namespace quoll
