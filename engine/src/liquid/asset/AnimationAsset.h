#pragma once

#include "liquid/scene/Joint.h"

namespace liquid {

enum class KeyframeSequenceAssetTarget : uint8_t { Position, Rotation, Scale };

enum class KeyframeSequenceAssetInterpolation : uint8_t { Step, Linear };

struct KeyframeSequenceAsset {
  std::vector<float> keyframeTimes;
  std::vector<glm::vec4> keyframeValues;

  JointId joint = 0;
  bool jointTarget = false;
  KeyframeSequenceAssetTarget target = KeyframeSequenceAssetTarget::Position;
  KeyframeSequenceAssetInterpolation interpolation =
      KeyframeSequenceAssetInterpolation::Step;
};

struct AnimationAsset {
  float time = 0.0f;
  std::vector<KeyframeSequenceAsset> keyframes;
};

} // namespace liquid
