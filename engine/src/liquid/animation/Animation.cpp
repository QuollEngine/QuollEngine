#include "liquid/core/Base.h"

#include "Animation.h"

namespace liquid {

Animation::Animation(const String &name, float time)
    : mName(name), mTime(time) {}

void Animation::addKeyframeSequence(const KeyframeSequence &sequence) {
  mKeyframeSequences.push_back(sequence);
}

} // namespace liquid
