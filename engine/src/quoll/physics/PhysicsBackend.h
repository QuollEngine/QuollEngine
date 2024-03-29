#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "CollisionHit.h"
#include "PhysicsSignals.h"

namespace quoll {

class PhysicsSignals;

class PhysicsBackend : NoCopyMove {
public:
  PhysicsBackend() = default;

  virtual ~PhysicsBackend() = default;

  virtual void update(f32 dt, EntityDatabase &entityDatabase) = 0;

  virtual void cleanup(EntityDatabase &entityDatabase) = 0;

  virtual void observeChanges(EntityDatabase &entityDatabase) = 0;

  virtual bool sweep(EntityDatabase &entityDatabase, Entity entity,
                     const glm::vec3 &direction, f32 distance,
                     CollisionHit &hit) = 0;

  virtual PhysicsSignals &getSignals() = 0;
};

} // namespace quoll
