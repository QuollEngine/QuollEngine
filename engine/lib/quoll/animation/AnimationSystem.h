#pragma once

#include "KeyframeInterpolator.h"

namespace quoll {

class AssetRegistry;
struct SystemView;

class AnimationSystem {
public:
  void prepare(SystemView &view);

  void update(f32 dt, SystemView &view);

private:
  KeyframeInterpolator mKeyframeInterpolator;
};

} // namespace quoll
