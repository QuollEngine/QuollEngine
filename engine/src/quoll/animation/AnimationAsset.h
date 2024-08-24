#pragma once

#include "quoll/skeleton/Joint.h"

namespace quoll {

enum class KeyframeSequenceAssetTarget : u8 { Position, Rotation, Scale };

enum class KeyframeSequenceAssetInterpolation : u8 { Step, Linear };

struct KeyframeSequenceAsset {
  std::vector<f32> keyframeTimes;

  std::vector<glm::vec4> keyframeValues;

  JointId joint = 0;

  bool jointTarget = false;

  KeyframeSequenceAssetTarget target = KeyframeSequenceAssetTarget::Position;

  KeyframeSequenceAssetInterpolation interpolation =
      KeyframeSequenceAssetInterpolation::Step;
};

struct AnimationAsset {
  f32 time = 0.0f;

  std::vector<KeyframeSequenceAsset> keyframes;
};

} // namespace quoll
