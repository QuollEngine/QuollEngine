#include "liquid/core/Base.h"

#include "KeyframeSequence.h"

namespace liquid {

KeyframeSequence::KeyframeSequence(KeyframeSequenceTarget target,
                                   KeyframeSequenceInterpolation interpolation)
    : mTarget(target), mInterpolation(interpolation) {}

KeyframeSequence::KeyframeSequence(KeyframeSequenceTarget target,
                                   KeyframeSequenceInterpolation interpolation,
                                   JointId joint)
    : mTarget(target), mInterpolation(interpolation), mJoint(joint),
      mJointTarget(true) {}

void KeyframeSequence::addKeyframe(float time, glm::vec4 value) {
  LIQUID_ASSERT(time >= 0.0f && time <= 1.0f,
                "Normalized time must be between [0,1]");

  mKeyframeTimes.push_back(time);
  mKeyframeValues.push_back(value);
}

glm::vec4 KeyframeSequence::getInterpolatedValue(float time) const {
  LIQUID_ASSERT(mKeyframeTimes.size() > 0, "No keyframes found");

  if (mInterpolation == KeyframeSequenceInterpolation::Step) {
    return getStepInterpolatedValue(time);
  }

  return getLinearInterpolatedValue(time);
}

glm::vec4 KeyframeSequence::getStepInterpolatedValue(float time) const {
  size_t found = 0;

  for (size_t i = 0; i < mKeyframeTimes.size(); ++i) {
    if (time < mKeyframeTimes.at(i)) {
      break;
    }

    found = i;
  }

  return mKeyframeValues.at(found);
}

glm::vec4 KeyframeSequence::getLinearInterpolatedValue(float time) const {
  size_t found = 0;

  for (size_t i = 0; i < mKeyframeTimes.size(); ++i) {
    if (time < mKeyframeTimes.at(i)) {
      break;
    }

    found = i;
  }

  if (found == mKeyframeTimes.size() - 1) {
    return mKeyframeValues.at(found);
  }

  const auto &currentVal = mKeyframeValues.at(found);
  const auto &nextVal = mKeyframeValues.at(found + 1);

  const auto &currentTime = mKeyframeTimes.at(found);
  const auto &nextTime = mKeyframeTimes.at(found + 1);

  glm::vec4 k = (nextVal - currentVal) / (nextTime - currentTime);

  return currentVal + k * (time - currentTime);
}

} // namespace liquid
