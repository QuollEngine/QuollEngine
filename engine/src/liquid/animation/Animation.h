#pragma once

#include "KeyframeSequence.h"

namespace liquid {

class Animation {
public:
  /**
   * @brief Create animation
   *
   * @param name Animation name
   * @param time Animation time
   */
  Animation(const String &name, float time);

  /**
   * @brief Add keyframe sequence
   *
   * @param sequence Keyframe sequence
   */
  void addKeyframeSequence(const KeyframeSequence &sequence);

  /**
   * @brief Get animation name
   *
   * @return Animation name
   */
  inline const String &getName() const { return name; }

  /**
   * @brief Get animation time
   *
   * @return Animation time
   */
  inline float getTime() const { return time; }

  /**
   * @brief Get keyframe sequences
   *
   * @return Keyframe sequences
   */
  inline const std::vector<KeyframeSequence> &getKeyframeSequences() const {
    return keyframeSequences;
  }

private:
  String name;
  float time;
  std::vector<KeyframeSequence> keyframeSequences;
};

} // namespace liquid
