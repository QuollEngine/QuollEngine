#pragma once

#include "quoll/signals/Signal.h"

namespace quoll::lua {

/**
 * @brief Script loop
 */
class ScriptLoop {
public:
  /**
   * @brief Get update signal
   *
   * @return Update signal
   */
  inline Signal<f32> &onUpdate() { return mOnUpdateSignal; }

private:
  Signal<f32> mOnUpdateSignal;
};

} // namespace quoll::lua
