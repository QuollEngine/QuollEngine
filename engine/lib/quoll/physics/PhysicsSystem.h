#pragma once

#include "PhysicsBackend.h"

namespace quoll {

class PhysicsSystem {
public:
  PhysicsSystem(PhysicsBackend *backend);

  constexpr void update(f32 dt, SystemView &view) {
    mBackend->update(dt, view);
  }

  constexpr void cleanup(SystemView &view) { mBackend->cleanup(view); }

  constexpr void createSystemViewData(SystemView &view) {
    mBackend->createSystemViewData(view);
  }

  constexpr bool sweep(EntityDatabase &entityDatabase, Entity entity,
                       const glm::vec3 &direction, f32 maxDistance,
                       CollisionHit &hit) {
    return mBackend->sweep(entityDatabase, entity, direction, maxDistance, hit);
  }

  constexpr PhysicsSignals &getSignals() { return mBackend->getSignals(); }

  constexpr debug::DebugPanel *getDebugPanel() {
    return mBackend->getDebugPanel();
  }

private:
  std::unique_ptr<PhysicsBackend> mBackend;
};

} // namespace quoll
