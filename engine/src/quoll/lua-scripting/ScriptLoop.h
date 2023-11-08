#pragma once

#include "ScriptSignal.h"

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
  inline ScriptSignal &getUpdateSignal() { return mUpdateSignal; }

private:
  ScriptSignal mUpdateSignal;
};

} // namespace quoll::lua
