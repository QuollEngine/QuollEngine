#pragma once

#include <PxConfig.h>
#include <PxShape.h>
#include <PxMaterial.h>

namespace quoll {

/**
 * @brief PhysX instance component
 */
struct PhysxInstance {
  /**
   * Dynamic rigid body actor
   */
  physx::PxRigidDynamic *rigidDynamic = nullptr;

  /**
   * Static rigid body actor
   */
  physx::PxRigidStatic *rigidStatic = nullptr;

  /**
   * Collidable shape
   */
  physx::PxShape *shape = nullptr;

  /**
   * Material
   */
  physx::PxMaterial *material = nullptr;
};

} // namespace quoll
