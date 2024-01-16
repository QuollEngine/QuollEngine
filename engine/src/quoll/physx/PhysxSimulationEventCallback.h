#pragma once

#include "quoll/physics/PhysicsSignals.h"
#include <PxConfig.h>
#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>

namespace quoll {

class PhysxSimulationEventCallback : public physx::PxSimulationEventCallback {
public:
  PhysxSimulationEventCallback(PhysicsSignals &signals);

  void onConstraintBreak(physx::PxConstraintInfo *constraints,
                         physx::PxU32 count) override;

  void onWake(physx::PxActor **actors, physx::PxU32 count) override;

  void onSleep(physx::PxActor **actors, physx::PxU32 count) override;

  void onContact(const physx::PxContactPairHeader &pairHeader,
                 const physx::PxContactPair *pairs,
                 physx::PxU32 nbPairs) override;
  void onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count) override;

  void onAdvance(const physx::PxRigidBody *const *bodyBuffer,
                 const physx::PxTransform *poseBuffer,
                 const physx::PxU32 count) override;

private:
  PhysicsSignals &mSignals;
};

} // namespace quoll
