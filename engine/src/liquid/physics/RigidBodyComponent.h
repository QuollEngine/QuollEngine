#pragma once

#include <PxConfig.h>
#include <PxRigidDynamic.h>

#include "PhysicsObjects.h"

namespace liquid {

/**
 * @brief Rigid body component
 */
struct RigidBodyComponent {
  /**
   * Dynamic rigid body description
   */
  PhysicsDynamicRigidBodyDesc dynamicDesc;

  /**
   * PhysX Rigid body actor
   */
  physx::PxRigidDynamic *actor = nullptr;
};

} // namespace liquid
