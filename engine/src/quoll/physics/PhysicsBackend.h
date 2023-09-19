#pragma once

#include "quoll/entity/EntityDatabase.h"

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
};

} // namespace quoll
