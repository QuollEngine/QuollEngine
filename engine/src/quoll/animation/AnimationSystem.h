#pragma once

#include "KeyframeInterpolator.h"

namespace quoll {

class AssetRegistry;
struct SystemView;

class AnimationSystem {
public:
  AnimationSystem(AssetRegistry &assetRegistry);

  void createSystemViewData(SystemView &view);

  void prepare(SystemView &view);

  void update(f32 dt, SystemView &view);

private:
  AssetRegistry &mAssetRegistry;
  KeyframeInterpolator mKeyframeInterpolator;
};

} // namespace quoll
