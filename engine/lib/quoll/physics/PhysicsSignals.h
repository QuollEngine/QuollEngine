#pragma once

#include "quoll/signals/Signal.h"
#include "CollisionEvent.h"

namespace quoll {

class PhysicsSignals {
public:
  inline Signal<CollisionEvent> &onCollisionStart() {
    return mOnCollisionStartSignal;
  }

  inline Signal<CollisionEvent> &onCollisionEnd() {
    return mOnCollisionEndSignal;
  }

private:
  Signal<CollisionEvent> mOnCollisionStartSignal;
  Signal<CollisionEvent> mOnCollisionEndSignal;
};

} // namespace quoll
