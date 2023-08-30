#pragma once

#include "quoll/asset/AnimationAsset.h"

namespace quoll {

/**
 * @brief Interpolates keyframes
 */
class KeyframeInterpolator {
public:
  /**
   * @brief Get vec3 interpolated value based on keyframe sequence
   *
   * @param sequence Keyframe sequence
   * @param time Time
   * @return Vec3 value
   */
  static glm::vec3 interpolateVec3(const KeyframeSequenceAsset &sequence,
                                   float time);

  /**
   * @brief Get quat interpolated value based on keyframe sequence
   *
   * @param sequence Keyframe sequence
   * @param time Time
   * @return Quat value
   */
  static glm::quat interpolateQuat(const KeyframeSequenceAsset &sequence,
                                   float time);
};

} // namespace quoll
