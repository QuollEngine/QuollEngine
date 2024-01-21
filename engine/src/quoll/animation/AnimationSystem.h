#pragma once

#include "KeyframeInterpolator.h"

namespace quoll {

class AssetRegistry;
class EntityDatabase;

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
