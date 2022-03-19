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
  inline const String &getName() const { return mName; }

  /**
   * @brief Get animation time
   *
   * @return Animation time
   */
  inline float getTime() const { return mTime; }

  /**
   * @brief Get keyframe sequences
   *
   * @return Keyframe sequences
   */
  inline const std::vector<KeyframeSequence> &getKeyframeSequences() const {
    return mKeyframeSequences;
  }

private:
  String mName;
  float mTime;
  std::vector<KeyframeSequence> mKeyframeSequences;
};

} // namespace liquid
