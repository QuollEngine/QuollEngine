#pragma once

#include "liquid/scene/Joint.h"

namespace liquid {

enum class KeyframeSequenceAssetTarget { Position, Rotation, Scale };

enum class KeyframeSequenceAssetInterpolation { Step, Linear };

struct KeyframeSequenceAsset {
  std::vector<float> keyframeTimes;
  std::vector<glm::vec4> keyframeValues;

  JointId joint = 0;
  bool jointTarget = false;
  KeyframeSequenceAssetTarget target;
  KeyframeSequenceAssetInterpolation interpolation;
};

struct AnimationAsset {
  float time = 0.0f;
  std::vector<KeyframeSequenceAsset> keyframes;
};

} // namespace liquid
