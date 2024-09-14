#pragma once

namespace quoll {

struct KeyframeSequenceAsset;

class KeyframeInterpolator {
public:
  static glm::vec3 interpolateVec3(const KeyframeSequenceAsset &sequence,
                                   f32 time);

  static glm::quat interpolateQuat(const KeyframeSequenceAsset &sequence,
                                   f32 time);
};

} // namespace quoll
