#pragma once

#include "PhysicsBackend.h"

namespace quoll {

class PhysicsSystem {
public:
  PhysicsSystem(PhysicsBackend *backend);

  inline void update(f32 dt, SystemView &view) { mBackend->update(dt, view); }

  inline void cleanup(SystemView &view) { mBackend->cleanup(view); }

  inline void createSystemViewData(SystemView &view) {
    mBackend->createSystemViewData(view);
  }

  inline bool sweep(EntityDatabase &entityDatabase, Entity entity,
                    const glm::vec3 &direction, f32 maxDistance,
                    CollisionHit &hit) {
    return mBackend->sweep(entityDatabase, entity, direction, maxDistance, hit);
  }

  inline PhysicsSignals &getSignals() { return mBackend->getSignals(); }

private:
  std::unique_ptr<PhysicsBackend> mBackend;
};

} // namespace quoll
