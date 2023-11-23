#pragma once

#include "quoll/lua-scripting/ScriptSignal.h"

namespace quoll {

/**
 * @brief Window signals
 */
class PhysicsSignals {
public:
  /**
   * @brief Get collision start signal
   *
   * @return Collision start signal
   */
  inline lua::ScriptSignal &getCollisionStartSignal() {
    return mCollisionStartSignal;
  }

  /**
   * @brief Get collision end signal
   *
   * @return Collision end signal
   */
  inline lua::ScriptSignal &getCollisionEndSignal() {
    return mCollisionEndSignal;
  }

private:
  lua::ScriptSignal mCollisionStartSignal;
  lua::ScriptSignal mCollisionEndSignal;
};

} // namespace quoll
