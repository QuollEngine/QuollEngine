#pragma once

#include "quoll/signals/Signal.h"
#include "CollisionEvent.h"

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
  inline Signal<CollisionEvent> &onCollisionStart() {
    return mOnCollisionStartSignal;
  }

  /**
   * @brief Get collision end signal
   *
   * @return Collision end signal
   */
  inline Signal<CollisionEvent> &onCollisionEnd() {
    return mOnCollisionEndSignal;
  }

private:
  Signal<CollisionEvent> mOnCollisionStartSignal;
  Signal<CollisionEvent> mOnCollisionEndSignal;
};

} // namespace quoll
