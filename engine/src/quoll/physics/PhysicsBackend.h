#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "CollisionHit.h"

namespace quoll {

/**
 * @brief Physics backend interface
 */
class PhysicsBackend {
public:
  /**
   * @brief Create physics backend
   */
  PhysicsBackend() = default;

  /**
   * @brief Destructor
   */
  virtual ~PhysicsBackend() = default;

  PhysicsBackend(const PhysicsBackend &) = delete;
  PhysicsBackend &operator=(const PhysicsBackend &) = delete;
  PhysicsBackend(PhysicsBackend &&) = delete;
  PhysicsBackend &operator=(PhysicsBackend &&) = delete;

  /**
   * @brief Update physics
   *
   * Performs physics simulation
   *
   * @param dt Time delta
   * @param entityDatabase Entity database
   */
  virtual void update(float dt, EntityDatabase &entityDatabase) = 0;

  /**
   * @brief Cleanup physics data
   *
   * @param entityDatabase Entity database
   */
  virtual void cleanup(EntityDatabase &entityDatabase) = 0;

  /**
   * @brief Observer changes in entities
   *
   * @param entityDatabase Entity database
   */
  virtual void observeChanges(EntityDatabase &entityDatabase) = 0;

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
  virtual bool sweep(EntityDatabase &entityDatabase, Entity entity,
                     const glm::vec3 &direction, float distance,
                     CollisionHit &hit) = 0;
};

} // namespace quoll
