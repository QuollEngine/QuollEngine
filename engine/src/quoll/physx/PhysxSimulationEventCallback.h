#pragma once

#include <PxConfig.h>
#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>

#include "quoll/events/EventSystem.h"

namespace quoll {

/**
 * @brief Physx simulation event callback
 *
 * Used for finding collisions
 */
class PhysxSimulationEventCallback : public physx::PxSimulationEventCallback {
public:
  /**
   * @brief Create simulation event callback
   *
   * @param eventSystem Event system
   */
  PhysxSimulationEventCallback(EventSystem &eventSystem);

  /**
   * @brief Event when constraint is broken
   *
   * @param constraints Constraints
   * @param count Number of constraints
   */
  void onConstraintBreak(physx::PxConstraintInfo *constraints,
                         physx::PxU32 count) override;

  /**
   * @brief Event when actors are awoken
   *
   * @param actors Actors
   * @param count Number of actors
   */
  void onWake(physx::PxActor **actors, physx::PxU32 count) override;

  /**
   * @brief Event when actors are asleep
   *
   * @param actors Actors
   * @param count Number of actors
   */
  void onSleep(physx::PxActor **actors, physx::PxU32 count) override;

  /**
   * @brief Event when actors are in contact
   *
   * @param pairHeader Contact pair header
   * @param pairs Contact pairs
   * @param nbPairs Number of pairs
   */
  void onContact(const physx::PxContactPairHeader &pairHeader,
                 const physx::PxContactPair *pairs,
                 physx::PxU32 nbPairs) override;
  /**
   * @brief Event when actors are triggered
   *
   * @param pairs Trigger pairs
   * @param count Number of pairs
   */
  void onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count) override;

  /**
   * @brief Event when simulation is running
   *
   * @param bodyBuffer Rigid body buffer
   * @param poseBuffer Pose buffer
   * @param count Number of items
   */
  void onAdvance(const physx::PxRigidBody *const *bodyBuffer,
                 const physx::PxTransform *poseBuffer,
                 const physx::PxU32 count) override;

private:
  EventSystem &mEventSystem;
};

} // namespace quoll
