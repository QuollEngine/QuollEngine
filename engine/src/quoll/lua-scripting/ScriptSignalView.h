#pragma once

#include "ScriptSignal.h"

namespace quoll::lua {

/**
 * @brief Script signal view
 *
 * Creates view for signal with
 * reference to entity script
 */
class ScriptSignalView {
  using Handler = sol::function;

public:
  /**
   * @brief Create signal view
   *
   * @param signal Signal
   * @param script Script
   */
  ScriptSignalView(ScriptSignal &signal, LuaScript &script);

  /**
   * @brief Connect new handler
   *
   * @param handler Handler
   * @return Signal slot
   */
  ScriptSignalSlot connect(Handler handler);

private:
  ScriptSignal &mSignal;
  LuaScript &mScript;
};

} // namespace quoll::lua
