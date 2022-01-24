#include "core/Base.h"

#include "KeyframeSequence.h"

namespace liquid {

KeyframeSequence::KeyframeSequence(KeyframeSequenceTarget target_,
                                   KeyframeSequenceInterpolation interpolation_)
    : target(target_), interpolation(interpolation_) {}

void KeyframeSequence::addKeyframe(float time, glm::vec4 value) {
  LIQUID_ASSERT(time >= 0.0f && time <= 1.0f,
                "Normalized time must be between [0,1]");

  keyframeTimes.push_back(time);
  keyframeValues.push_back(value);
}

glm::vec4 KeyframeSequence::getInterpolatedValue(float time) const {
  LIQUID_ASSERT(keyframeTimes.size() > 0, "No keyframes found");
  size_t found = 0;

  for (size_t i = 0; i < keyframeTimes.size(); ++i) {
    if (time < keyframeTimes.at(i)) {
      break;
    }

    found = i;
  }

  return keyframeValues.at(found);
}

} // namespace liquid
