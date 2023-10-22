#include "quoll/core/Base.h"
#include "KeyframeInterpolator.h"

namespace quoll {

/**
 * @brief Convert vec4 to other values
 *
 * @tparam T Converted value type
 * @param value Vec4 value
 * @return Converted value
 */
template <class T> T convertVec4(const glm::vec4 &value) { return T(value); }

/**
 * @brief Convert vec4 to quat
 *
 * @param value Vec4 value
 * @return Converted quat value
 */
template <> glm::quat convertVec4(const glm::vec4 &value) {
  return glm::quat(value.w, value.x, value.y, value.z);
}

/**
 * @brief Linear interpolation
 *
 * @tparam T Interpolated value type
 * @param a First value
 * @param b Second value
 * @param k Interpolation constant
 * @return Interpolated value
 */
template <class T> T interpolateLinear(const T &a, const T &b, f32 k) {
  return a + k * (b - a);
}

/**
 * @brief Spherical linear interpolation
 *
 * Accepts quaternions
 *
 * @param a First value
 * @param b Second value
 * @param k Interpolation constant
 * @return Interpolated value
 */
template <>
glm::quat interpolateLinear(const glm::quat &a, const glm::quat &b, f32 k) {
  return glm::normalize(glm::slerp(a, b, k));
}

/**
 * @brief Get step interpolation value
 *
 * @tparam T Interpolated value type
 * @param sequence Keyframe sequence
 * @param time Normalized time
 * @return Step interpolated value
 */
template <class T>
T getStepValue(const KeyframeSequenceAsset &sequence, f32 time) {
  usize found = 0;

  for (usize i = 0; i < sequence.keyframeTimes.size(); ++i) {
    if (time < sequence.keyframeTimes.at(i)) {
      break;
    }

    found = i;
  }

  return convertVec4<T>(sequence.keyframeValues.at(found));
}

/**
 * @brief Get linear interpolation value
 *
 * @tparam T Interpolated value type
 * @param sequence Keyframe sequence
 * @param time Normalized time
 * @return Linear interpolated value
 */
template <class T>
const T getLinearValue(const KeyframeSequenceAsset &sequence, f32 time) {
  usize found = 0;

  for (usize i = 0; i < sequence.keyframeTimes.size(); ++i) {
    if (time < sequence.keyframeTimes.at(i)) {
      break;
    }

    found = i;
  }

  if (found == sequence.keyframeTimes.size() - 1) {
    return convertVec4<T>(sequence.keyframeValues.at(found));
  }

  const auto &currentVal = convertVec4<T>(sequence.keyframeValues.at(found));
  const auto &nextVal = convertVec4<T>(sequence.keyframeValues.at(found + 1));

  const auto &currentTime = sequence.keyframeTimes.at(found);
  const auto &nextTime = sequence.keyframeTimes.at(found + 1);

  f32 k = (time - currentTime) / (nextTime - currentTime);

  return interpolateLinear(currentVal, nextVal, k);
}

/**
 * @brief Interpolate value
 *
 * @tparam T Interpolated value type
 * @param sequence Keyframe sequence
 * @param time Normalized time
 * @return Interpolated value
 */
template <class T>
T interpolate(const KeyframeSequenceAsset &sequence, f32 time) {
  if (sequence.interpolation == KeyframeSequenceAssetInterpolation::Step) {
    return getStepValue<T>(sequence, time);
  }

  return getLinearValue<T>(sequence, time);
}

glm::vec3
KeyframeInterpolator::interpolateVec3(const KeyframeSequenceAsset &sequence,
                                      f32 time) {
  return interpolate<glm::vec3>(sequence, time);
}

glm::quat
KeyframeInterpolator::interpolateQuat(const KeyframeSequenceAsset &sequence,
                                      f32 time) {
  return interpolate<glm::quat>(sequence, time);
}

} // namespace quoll
