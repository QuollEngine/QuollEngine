#pragma once

#include <PxConfig.h>
#include <PxShape.h>
#include <PxMaterial.h>

#include "PhysicsObjects.h"

namespace liquid {

/**
 * @brief Collidable component
 */
struct Collidable {
  /**
   * Geometry description
   */
  PhysicsGeometryDesc geometryDesc;

  /**
   * Physics material description
   */
  PhysicsMaterialDesc materialDesc;

  /**
   * PhysX shape
   */
  physx::PxShape *shape = nullptr;

  /**
   * PhysX material
   */
  physx::PxMaterial *material = nullptr;

  /**
   * PhysX rigid body
   */
  physx::PxRigidStatic *rigidStatic = nullptr;
};

} // namespace liquid
