#pragma once

#include "PhysicsObjects.h"

namespace quoll {

/**
 * @brief Rigid body component
 */
struct RigidBody {
  /**
   * Dynamic rigid body description
   */
  PhysicsDynamicRigidBodyDesc dynamicDesc;
};

} // namespace quoll
