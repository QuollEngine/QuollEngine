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
  inline Signal<CollisionEvent> &getCollisionStartSignal() {
    return mCollisionStartSignal;
  }

  /**
   * @brief Get collision end signal
   *
   * @return Collision end signal
   */
  inline Signal<CollisionEvent> &getCollisionEndSignal() {
    return mCollisionEndSignal;
  }

private:
  Signal<CollisionEvent> mCollisionStartSignal;
  Signal<CollisionEvent> mCollisionEndSignal;
};

} // namespace quoll
