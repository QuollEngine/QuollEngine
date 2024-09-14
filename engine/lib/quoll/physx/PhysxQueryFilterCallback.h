#pragma once

#include <PxPhysics.h>
#include <PxQueryFiltering.h>

namespace quoll {

class PhysxQueryFilterCallback : public physx::PxQueryFilterCallback {
public:
  PhysxQueryFilterCallback(physx::PxShape *shape);

  physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData &filterData,
                                        const physx::PxShape *shape,
                                        const physx::PxRigidActor *actor,
                                        physx::PxHitFlags &queryFlags) override;

  physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData &filterData,
                                         const physx::PxQueryHit &hit) override;

private:
  physx::PxShape *mShape = nullptr;
};

} // namespace quoll
