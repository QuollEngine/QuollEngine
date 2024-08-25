#pragma once

#include "quoll/asset/AssetHandle.h"
#include "AnimatorAsset.h"

namespace quoll {

/**
 * Animator component that handles
 * entity specific state machine
 * for controlling animations
 *
 * The animator works with normalized time
 * in range of [0.0, 1.0].
 */
struct Animator {
  AssetHandle<AnimatorAsset> asset;

  usize currentState = std::numeric_limits<usize>::max();

  f32 normalizedTime = 0.0f;

  bool playing = true;
};

} // namespace quoll
