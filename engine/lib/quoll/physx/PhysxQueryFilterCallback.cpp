#include "quoll/core/Base.h"
#include "PhysxQueryFilterCallback.h"

namespace quoll {

PhysxQueryFilterCallback::PhysxQueryFilterCallback(physx::PxShape *shape)
    : mShape(shape) {}

physx::PxQueryHitType::Enum PhysxQueryFilterCallback::preFilter(
    const physx::PxFilterData &filterData, const physx::PxShape *shape,
    const physx::PxRigidActor *actor, physx::PxHitFlags &queryFlags) {

  if (shape == mShape) {
    return physx::PxQueryHitType::eNONE;
  }

  return physx::PxQueryHitType::eBLOCK;
}

physx::PxQueryHitType::Enum
PhysxQueryFilterCallback::postFilter(const physx::PxFilterData &filterData,
                                     const physx::PxQueryHit &hit) {
  return physx::PxQueryHitType::eBLOCK;
}

} // namespace quoll
