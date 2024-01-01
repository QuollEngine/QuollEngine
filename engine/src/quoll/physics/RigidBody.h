#pragma once

#include "PhysicsObjects.h"

namespace quoll {

enum class RigidBodyType { Dynamic, Kinematic };

/**
 * @brief Rigid body component
 */
struct RigidBody {
  /**
   * Rigid body type
   */
  RigidBodyType type{RigidBodyType::Dynamic};

  /**
   * Dynamic rigid body description
   */
  PhysicsDynamicRigidBodyDesc dynamicDesc;
};

} // namespace quoll
