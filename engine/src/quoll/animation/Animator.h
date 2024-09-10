#pragma once

#include "quoll/asset/AssetRef.h"
#include "AnimatorAsset.h"

namespace quoll {

struct AnimatorAssetRef {
  AssetRef<AnimatorAsset> asset;
};

struct AnimatorCurrentAsset {
  AssetHandle<AnimatorAsset> handle;
};

struct Animator {
  AssetRef<AnimatorAsset> asset;

  usize currentState = std::numeric_limits<usize>::max();

  f32 normalizedTime = 0.0f;

  bool playing = true;
};

} // namespace quoll
