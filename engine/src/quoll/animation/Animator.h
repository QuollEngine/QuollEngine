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
  size_t currentState = std::numeric_limits<size_t>::max();

  /**
   * Normalized time
   *
   * Range: [0.0, 1.0];
   */
  float normalizedTime = 0.0f;

  /**
   * Animation is playing
   */
  bool playing = true;
};

} // namespace quoll
