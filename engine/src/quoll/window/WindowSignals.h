#pragma once

#include "quoll/lua-scripting/ScriptSignal.h"

namespace quoll {

/**
 * @brief Window signals
 */
class WindowSignals {
public:
  /**
   * @brief Get key down signal
   *
   * @return Key down signal
   */
  inline lua::ScriptSignal &getKeyDownSignal() { return mKeyDownSignal; }

  /**
   * @brief Get key up signal
   *
   * @return Key up signal
   */
  inline lua::ScriptSignal &getKeyUpSignal() { return mKeyUpSignal; }

private:
  lua::ScriptSignal mKeyDownSignal;
  lua::ScriptSignal mKeyUpSignal;
};

} // namespace quoll
