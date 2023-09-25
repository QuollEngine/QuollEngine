#pragma once

#include "PhysicsBackend.h"
#include "quoll/events/EventSystem.h"

namespace quoll {

/**
 * @brief Physics system
 */
class PhysicsSystem {
public:
  /**
   * @brief Create physx backend
   *
   * @param eventSystem Event system
   * @return Physx backend
   */
  static PhysicsSystem createPhysxBackend(EventSystem &eventSystem);

public:
  /**
   * @brief Create physics system
   *
   * @param backend Physics backend
   */
  PhysicsSystem(PhysicsBackend *backend);

  /**
   * @brief Update physics
   *
   * Performs physics simulation
   *
   * @param dt Time delta
   * @param entityDatabase Entity database
   */
  inline void update(float dt, EntityDatabase &entityDatabase) {
    mBackend->update(dt, entityDatabase);
  }

  /**
   * @brief Cleanup physics data
   *
   * @param entityDatabase Entity database
   */
  inline void cleanup(EntityDatabase &entityDatabase) {
    mBackend->cleanup(entityDatabase);
  }

  /**
   * @brief Observer changes in entities
   *
   * @param entityDatabase Entity database
   */
  inline void observeChanges(EntityDatabase &entityDatabase) {
    mBackend->observeChanges(entityDatabase);
  }

  /**
   * @brief Run sweep collision test
   *
   * @param entityDatabase Entity database
   * @param entity Entity
   * @param direction Sweep direction
   * @param distance Sweep distance
   * @param[out] hit Collision hit
   * @retval true Entity collided
   * @retval false Entity not collided
   */
  inline bool sweep(EntityDatabase &entityDatabase, Entity entity,
                    const glm::vec3 &direction, float distance,
                    CollisionHit &hit) {
    return mBackend->sweep(entityDatabase, entity, direction, distance, hit);
  }

private:
  std::unique_ptr<PhysicsBackend> mBackend;
};

} // namespace quoll
