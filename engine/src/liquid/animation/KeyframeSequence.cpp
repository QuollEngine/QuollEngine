#include "liquid/core/Base.h"

#include "KeyframeSequence.h"

namespace liquid {

KeyframeSequence::KeyframeSequence(KeyframeSequenceTarget target_,
                                   KeyframeSequenceInterpolation interpolation_)
    : target(target_), interpolation(interpolation_) {}

KeyframeSequence::KeyframeSequence(KeyframeSequenceTarget target_,
                                   KeyframeSequenceInterpolation interpolation_,
                                   JointId joint_)
    : target(target_), interpolation(interpolation_), joint(joint_),
      jointTarget(true) {}

void KeyframeSequence::addKeyframe(float time, glm::vec4 value) {
  LIQUID_ASSERT(time >= 0.0f && time <= 1.0f,
                "Normalized time must be between [0,1]");

  keyframeTimes.push_back(time);
  keyframeValues.push_back(value);
}

glm::vec4 KeyframeSequence::getInterpolatedValue(float time) const {
  LIQUID_ASSERT(keyframeTimes.size() > 0, "No keyframes found");

  if (interpolation == KeyframeSequenceInterpolation::Step) {
    return getStepInterpolatedValue(time);
  }

  return getLinearInterpolatedValue(time);
}

glm::vec4 KeyframeSequence::getStepInterpolatedValue(float time) const {
  size_t found = 0;

  for (size_t i = 0; i < keyframeTimes.size(); ++i) {
    if (time < keyframeTimes.at(i)) {
      break;
    }

    found = i;
  }

  return keyframeValues.at(found);
}

glm::vec4 KeyframeSequence::getLinearInterpolatedValue(float time) const {
  size_t found = 0;

  for (size_t i = 0; i < keyframeTimes.size(); ++i) {
    if (time < keyframeTimes.at(i)) {
      break;
    }

    found = i;
  }

  if (found == keyframeTimes.size() - 1) {
    return keyframeValues.at(found);
  }

  const auto &currentVal = keyframeValues.at(found);
  const auto &nextVal = keyframeValues.at(found + 1);

  const auto &currentTime = keyframeTimes.at(found);
  const auto &nextTime = keyframeTimes.at(found + 1);

  glm::vec4 k = (nextVal - currentVal) / (nextTime - currentTime);

  return currentVal + k * (time - currentTime);
}

} // namespace liquid
