#pragma once

#include <PxPhysics.h>
#include <PxQueryFiltering.h>

namespace quoll {

/**
 * @brief Default Physx query filter callback
 *
 * Ignores the current shape during scene queries
 */
class PhysxQueryFilterCallback : public physx::PxQueryFilterCallback {
public:
  /**
   * @brief Create query filter callback
   *
   * @param shape Shape that is used for sweeping
   */
  PhysxQueryFilterCallback(physx::PxShape *shape);

  /**
   * @brief Prefilter callback
   *
   * @param filterData Filter data
   * @param shape Shape that is swept against
   * @param actor Rigid actor of the shape
   * @param queryFlags Query flags
   *
   * @return Hit type
   */
  physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData &filterData,
                                        const physx::PxShape *shape,
                                        const physx::PxRigidActor *actor,
                                        physx::PxHitFlags &queryFlags) override;

  /**
   * @brief Post filte rcallback
   *
   * @param filterData Filter data
   * @param hit Hit data
   *
   * @return Hit type
   */
  physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData &filterData,
                                         const physx::PxQueryHit &hit) override;

private:
  physx::PxShape *mShape = nullptr;
};

} // namespace quoll
