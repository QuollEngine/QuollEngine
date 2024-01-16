#pragma once

#include "PhysicsBackend.h"

namespace quoll {

class PhysicsSystem {
public:
  PhysicsSystem(PhysicsBackend *backend);

  inline void update(f32 dt, EntityDatabase &entityDatabase) {
    mBackend->update(dt, entityDatabase);
  }

  inline void cleanup(EntityDatabase &entityDatabase) {
    mBackend->cleanup(entityDatabase);
  }

  inline void observeChanges(EntityDatabase &entityDatabase) {
    mBackend->observeChanges(entityDatabase);
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
