#pragma once

#include "PhysicsObjects.h"

namespace liquid {

/**
 * @brief Rigid body component
 */
struct RigidBody {
  /**
   * Dynamic rigid body description
   */
  PhysicsDynamicRigidBodyDesc dynamicDesc;
};

} // namespace liquid
