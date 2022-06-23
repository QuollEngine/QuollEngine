#pragma once

#include "liquid/entity/EntityDatabase.h"
#include "liquid/events/EventSystem.h"

#include "PhysicsObjects.h"

namespace liquid {

/**
 * @brief Physics system
 */
class PhysicsSystem {
  class PhysicsSystemImpl;

public:
  /**
   * @brief Create physics system
   *
   * @param eventSystem Event system
   */
  PhysicsSystem(EventSystem &eventSystem);

  /**
   * @brief Destroy physics system
   */
  ~PhysicsSystem();

  PhysicsSystem(const PhysicsSystem &) = delete;
  PhysicsSystem &operator=(const PhysicsSystem &) = delete;
  PhysicsSystem(PhysicsSystem &&) = delete;
  PhysicsSystem &operator=(PhysicsSystem &&) = delete;

  /**
   * @brief Update physics
   *
   * Performs physics simulation
   *
   * @param dt Time delta
   * @param entityDatabase Entity database
   */
  void update(float dt, EntityDatabase &entityDatabase);

  /**
   * @brief Cleanup Physx actors and shapes
   *
   * @param entityDatabase Entity database
   */
  void cleanup(EntityDatabase &entityDatabase);

private:
  /**
   * @brief Synchronize physics components
   *
   * @param entityDatabase Entity database
   */
  void synchronizeComponents(EntityDatabase &entityDatabase);

  /**
   * @brief Synchronize transforms
   *
   * @param entityDatabase Entity database
   */
  void synchronizeTransforms(EntityDatabase &entityDatabase);

private:
  PhysicsSystemImpl *mImpl = nullptr;
  EventSystem &mEventSystem;
};

} // namespace liquid
