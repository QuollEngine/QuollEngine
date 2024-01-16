#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/EntityDatabase.h"
#include "KeyframeInterpolator.h"

namespace quoll {

class AnimationSystem {
public:
  AnimationSystem(AssetRegistry &assetRegistry);

  void prepare(EntityDatabase &entityDatabase);

  void update(f32 dt, EntityDatabase &entityDatabase);

private:
  AssetRegistry &mAssetRegistry;
  KeyframeInterpolator mKeyframeInterpolator;
};

} // namespace quoll
