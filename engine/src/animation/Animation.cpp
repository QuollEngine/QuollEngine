#include "core/Base.h"

#include "Animation.h"

namespace liquid {

Animation::Animation(const String &name_, float time_)
    : name(name_), time(time_) {}

void Animation::addKeyframeSequence(const KeyframeSequence &sequence) {
  keyframeSequences.push_back(sequence);
}

} // namespace liquid
