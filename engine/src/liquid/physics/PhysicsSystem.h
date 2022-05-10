#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/events/EventSystem.h"

#include "PhysicsObjects.h"

namespace liquid {

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
   * @param entityContext Entity context
   */
  void update(float dt, EntityContext &entityContext);

private:
  /**
   * @brief Synchronize physics components
   *
   * @param entityContext Entity context
   */
  void synchronizeComponents(EntityContext &entityContext);

  /**
   * @brief Synchronize transforms
   *
   * @param entityContext Entity context
   */
  void synchronizeTransforms(EntityContext &entityContext);

private:
  PhysicsSystemImpl *mImpl = nullptr;
  EventSystem &mEventSystem;
};

} // namespace liquid
