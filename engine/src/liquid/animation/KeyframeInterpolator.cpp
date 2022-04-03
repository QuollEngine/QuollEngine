#include "liquid/core/Base.h"
#include "KeyframeInterpolator.h"

namespace liquid {

const glm::vec4
KeyframeInterpolator::interpolate(const KeyframeSequenceAsset &sequence,
                                  float time) const {
  if (sequence.interpolation == KeyframeSequenceAssetInterpolation::Step) {
    return getStepInterpolatedValue(sequence, time);
  }

  return getLinearInterpolatedValue(sequence, time);
}

const glm::vec4 &KeyframeInterpolator::getStepInterpolatedValue(
    const KeyframeSequenceAsset &sequence, float time) const {
  size_t found = 0;

  for (size_t i = 0; i < sequence.keyframeTimes.size(); ++i) {
    if (time < sequence.keyframeTimes.at(i)) {
      break;
    }

    found = i;
  }

  return sequence.keyframeValues.at(found);
}

const glm::vec4 KeyframeInterpolator::getLinearInterpolatedValue(
    const KeyframeSequenceAsset &sequence, float time) const {
  size_t found = 0;

  for (size_t i = 0; i < sequence.keyframeTimes.size(); ++i) {
    if (time < sequence.keyframeTimes.at(i)) {
      break;
    }

    found = i;
  }

  if (found == sequence.keyframeTimes.size() - 1) {
    return sequence.keyframeValues.at(found);
  }

  const auto &currentVal = sequence.keyframeValues.at(found);
  const auto &nextVal = sequence.keyframeValues.at(found + 1);

  const auto &currentTime = sequence.keyframeTimes.at(found);
  const auto &nextTime = sequence.keyframeTimes.at(found + 1);

  glm::vec4 k = (nextVal - currentVal) / (nextTime - currentTime);

  return currentVal + k * (time - currentTime);
}

} // namespace liquid
