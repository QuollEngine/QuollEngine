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
  inline Signal<f32> &getUpdateSignal() { return mUpdateSignal; }

private:
  Signal<f32> mUpdateSignal;
};

} // namespace quoll::lua
