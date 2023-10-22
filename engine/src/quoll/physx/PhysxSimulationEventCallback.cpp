#include "quoll/core/Base.h"
#include "PhysxSimulationEventCallback.h"

using namespace physx;

namespace quoll {

PhysxSimulationEventCallback::PhysxSimulationEventCallback(
    EventSystem &eventSystem)
    : mEventSystem(eventSystem) {}

void PhysxSimulationEventCallback::onConstraintBreak(
    physx::PxConstraintInfo *constraints, physx::PxU32 count) {}

void PhysxSimulationEventCallback::onWake(physx::PxActor **actors,
                                          physx::PxU32 count) {}

void PhysxSimulationEventCallback::onSleep(physx::PxActor **actors,
                                           physx::PxU32 count) {}

void PhysxSimulationEventCallback::onContact(
    const physx::PxContactPairHeader &pairHeader,
    const physx::PxContactPair *pairs, physx::PxU32 nbPairs) {
  auto *actor1 = pairHeader.actors[0];
  auto *actor2 = pairHeader.actors[1];

  Entity e1 = static_cast<Entity>(reinterpret_cast<uptr>(actor1->userData));
  Entity e2 = static_cast<Entity>(reinterpret_cast<uptr>(actor2->userData));

  for (PxU32 i = 0; i < nbPairs; ++i) {
    const PxContactPair &cp = pairs[i];

    if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
      mEventSystem.dispatch(CollisionEvent::CollisionStarted, {e1, e2});
    } else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST) {
      mEventSystem.dispatch(CollisionEvent::CollisionEnded, {e1, e2});
    }
  }
}

void PhysxSimulationEventCallback::onTrigger(PxTriggerPair *pairs,
                                             PxU32 count) {}

void PhysxSimulationEventCallback::onAdvance(
    const PxRigidBody *const *bodyBuffer, const PxTransform *poseBuffer,
    const PxU32 count) {}

} // namespace quoll
