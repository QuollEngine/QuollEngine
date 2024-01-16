#pragma once

#include <PxConfig.h>
#include <PxMaterial.h>
#include <PxShape.h>

namespace quoll {

/**
 * Stores all initialized Physx objects
 * for entity. Used by PhysX backend
 */
struct PhysxInstance {
  physx::PxRigidDynamic *rigidDynamic = nullptr;

  physx::PxRigidStatic *rigidStatic = nullptr;

  physx::PxShape *shape = nullptr;

  physx::PxMaterial *material = nullptr;

  bool useShapeInSimulation = true;

  bool useShapeInQueries = true;
};

} // namespace quoll
