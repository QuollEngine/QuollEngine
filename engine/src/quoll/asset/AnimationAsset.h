#pragma once

#include "quoll/scene/Joint.h"

namespace quoll {

enum class KeyframeSequenceAssetTarget : u8 { Position, Rotation, Scale };

enum class KeyframeSequenceAssetInterpolation : u8 { Step, Linear };

/**
 * @brief Keyframe sequence asset
 */
struct KeyframeSequenceAsset {
  /**
   * List of keyframe times
   */
  std::vector<f32> keyframeTimes;

  /**
   * List of keyframe values
   */
  std::vector<glm::vec4> keyframeValues;

  /**
   * Joint ID
   */
  JointId joint = 0;

  /**
   * Is joint target
   */
  bool jointTarget = false;

  /**
   * Transform target
   */
  KeyframeSequenceAssetTarget target = KeyframeSequenceAssetTarget::Position;

  /**
   * Keyframe interpolation
   */
  KeyframeSequenceAssetInterpolation interpolation =
      KeyframeSequenceAssetInterpolation::Step;
};

/**
 * Animation asset data
 */
struct AnimationAsset {
  /**
   * Animation time
   */
  f32 time = 0.0f;

  /**
   * Keyframes
   */
  std::vector<KeyframeSequenceAsset> keyframes;
};

} // namespace quoll
