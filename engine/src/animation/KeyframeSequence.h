#pragma once

namespace liquid {

enum class KeyframeSequenceTarget { Position, Rotation, Scale };

enum class KeyframeSequenceInterpolation { Step };

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

private:
  KeyframeSequenceTarget target;
  KeyframeSequenceInterpolation interpolation;

  std::vector<float> keyframeTimes;
  std::vector<glm::vec4> keyframeValues;
};

} // namespace liquid