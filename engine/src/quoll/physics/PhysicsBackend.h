#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "CollisionHit.h"
#include "PhysicsSignals.h"

namespace quoll {

class PhysicsBackend {
public:
  PhysicsBackend() = default;

  virtual ~PhysicsBackend() = default;

  PhysicsBackend(const PhysicsBackend &) = delete;
  PhysicsBackend &operator=(const PhysicsBackend &) = delete;
  PhysicsBackend(PhysicsBackend &&) = delete;
  PhysicsBackend &operator=(PhysicsBackend &&) = delete;

  virtual void update(f32 dt, EntityDatabase &entityDatabase) = 0;

  virtual void cleanup(EntityDatabase &entityDatabase) = 0;

  virtual void observeChanges(EntityDatabase &entityDatabase) = 0;

  virtual bool sweep(EntityDatabase &entityDatabase, Entity entity,
                     const glm::vec3 &direction, f32 distance,
                     CollisionHit &hit) = 0;

  virtual PhysicsSignals &getSignals() = 0;
};

} // namespace quoll
