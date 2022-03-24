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
   * @param entityContext Entity context
   * @param eventSystem Event system
   */
  PhysicsSystem(EntityContext &entityContext, EventSystem &eventSystem);

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
   */
  void update(float dt);

private:
  /**
   * @brief Synchronize physics components
   */
  void synchronizeComponents();

  /**
   * @brief Synchronize transforms
   */
  void synchronizeTransforms();

private:
  PhysicsSystemImpl *mImpl = nullptr;
  EntityContext &mEntityContext;
  EventSystem &mEventSystem;
};

} // namespace liquid
