#pragma once

#include "liquid/scene/Joint.h"

namespace liquid {

enum class KeyframeSequenceTarget { Position, Rotation, Scale };

enum class KeyframeSequenceInterpolation { Step, Linear };

class KeyframeSequence {
public:
  /**
   * @brief Create keyframe sequence
   *
   * @param target Sequence target
   * @param interpolation Interpolation
   */
  KeyframeSequence(KeyframeSequenceTarget target,
                   KeyframeSequenceInterpolation interpolation);

  /**
   * @brief Create keyframe sequence for joint
   *
   * @param target Sequence target
   * @param interpolation Interpolation
   * @param joint Target joint
   */
  KeyframeSequence(KeyframeSequenceTarget target,
                   KeyframeSequenceInterpolation interpolation, JointId joint);

  /**
   * @brief Get sequence target
   *
   * @return Sequence target
   */
  inline KeyframeSequenceTarget getTarget() const { return target; }

  /**
   * @brief Get sequence interpolation
   *
   * @return Sequence interpolation
   */
  inline KeyframeSequenceInterpolation getInterpolation() const {
    return interpolation;
  }

  /**
   * @brief Add keyframe
   *
   * @param time Keyframe time
   * @param value Keyframe value
   */
  void addKeyframe(float time, glm::vec4 value);

  /**
   * @brief Get keyframe time
   *
   * @param index Keyframe index
   * @return Keyframe time
   */
  inline float getKeyframeTime(size_t index) const {
    return keyframeTimes.at(index);
  }

  /**
   * @brief Get keyframe value
   *
   * @param index Keyframe index
   * @return Keyframe value
   */
  inline const glm::vec4 &getKeyframeValue(size_t index) const {
    return keyframeValues.at(index);
  }

  /**
   * @brief Get interpolated value
   *
   * Returns previous keyframe value if interpolation is Step
   *
   * @param time Time
   * @return Interpolated value
   */
  glm::vec4 getInterpolatedValue(float time) const;

  /**
   * @brief Get joint
   *
   * @return joint
   */
  inline JointId getJoint() const { return joint; }

  /**
   * @brief Is joint target
   *
   * @retval true Is joint target
   * @retval false Is not joint target
   */
  inline bool isJointTarget() const { return jointTarget; }

private:
  /**
   * @brief Get value at previous time
   *
   * @param time ime
   * @return Value at previous time
   */
  glm::vec4 getStepInterpolatedValue(float time) const;

  /**
   * @brief Get linear interpolated value between two times
   *
   * @param time Time
   * @return Interpolated value
   */
  glm::vec4 getLinearInterpolatedValue(float time) const;

private:
  KeyframeSequenceTarget target;
  KeyframeSequenceInterpolation interpolation;
  JointId joint = 0;
  bool jointTarget = false;

  std::vector<float> keyframeTimes;
  std::vector<glm::vec4> keyframeValues;
};

} // namespace liquid