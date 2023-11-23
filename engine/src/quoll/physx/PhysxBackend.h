#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/physics/PhysicsObjects.h"
#include "quoll/physics/PhysicsBackend.h"

#include <PxConfig.h>
#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>

#include "PhysxInstance.h"
#include "PhysxSimulationEventCallback.h"

namespace quoll {

/**
 * @brief PhysX backend
 */
class PhysxBackend : public PhysicsBackend {
public:
  /**
   * @brief Create physics system
   */
  PhysxBackend();

  /**
   * @brief Destroy physics system
   */
  virtual ~PhysxBackend();

  PhysxBackend(const PhysxBackend &) = delete;
  PhysxBackend &operator=(const PhysxBackend &) = delete;
  PhysxBackend(PhysxBackend &&) = delete;
  PhysxBackend &operator=(PhysxBackend &&) = delete;

  /**
   * @brief Update physics
   *
   * Performs physics simulation
   *
   * @param dt Time delta
   * @param entityDatabase Entity database
   */
  void update(f32 dt, EntityDatabase &entityDatabase) override;

  /**
   * @brief Cleanup Physx actors and shapes
   *
   * @param entityDatabase Entity database
   */
  void cleanup(EntityDatabase &entityDatabase) override;

  /**
   * @brief Observer changes in entities
   *
   * @param entityDatabase Entity database
   */
  void observeChanges(EntityDatabase &entityDatabase) override;

  /**
   * @brief Run sweep collision test
   *
   * @param entityDatabase Entity database
   * @param entity Entity
   * @param direction Sweep direction
   * @param distance Sweep distance
   * @param[out] hit Collision hit
   * @retval true Entity collided
   * @retval false Entity not collided
   */
  bool sweep(EntityDatabase &entityDatabase, Entity entity,
             const glm::vec3 &direction, f32 distance,
             CollisionHit &hit) override;

  /**
   * @brief Get physics signals
   *
   * @return Physics signals
   */
  inline PhysicsSignals &getSignals() override { return mSignals; }

private:
  /**
   * @brief Synchronize physics components
   *
   * @param entityDatabase Entity database
   */
  void synchronizeComponents(EntityDatabase &entityDatabase);

  /**
   * @brief Synchronize transforms
   *
   * @param entityDatabase Entity database
   */
  void synchronizeTransforms(EntityDatabase &entityDatabase);

private:
  /**
   * @brief Create Physx shape
   *
   * @param geometryDesc Geometry description
   * @param material Physx material
   * @param worldTransform World transform
   * @return PhysX shape
   */
  physx::PxShape *createShape(const PhysicsGeometryDesc &geometryDesc,
                              physx::PxMaterial &material,
                              const glm::mat4 &worldTransform);

private:
  PhysicsSignals mSignals;

  physx::PxDefaultAllocator mDefaultAllocator;
  physx::PxDefaultErrorCallback mDefaultErrorCallback;
  PhysxSimulationEventCallback mSimulationEventCallback;

  physx::PxPvd *mPvd = nullptr;
  physx::PxFoundation *mFoundation = nullptr;
  physx::PxPhysics *mPhysics = nullptr;
  physx::PxDefaultCpuDispatcher *mDispatcher = nullptr;

  physx::PxScene *mScene = nullptr;

  EntityDatabaseObserver<PhysxInstance> mPhysxInstanceRemoveObserver;
};

} // namespace quoll
