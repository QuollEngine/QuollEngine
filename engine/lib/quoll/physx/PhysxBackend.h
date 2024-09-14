#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/physics/PhysicsBackend.h"
#include "quoll/physics/PhysicsObjects.h"
#include "PhysxInstance.h"
#include "PhysxSimulationEventCallback.h"
#include <PxConfig.h>
#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>

namespace quoll {

class PhysxBackend : public PhysicsBackend {
public:
  PhysxBackend();

  virtual ~PhysxBackend();

  void update(f32 dt, SystemView &view) override;

  void cleanup(SystemView &view) override;

  void createSystemViewData(SystemView &view) override;

  bool sweep(EntityDatabase &entityDatabase, Entity entity,
             const glm::vec3 &direction, f32 maxDistance,
             CollisionHit &hit) override;

  inline PhysicsSignals &getSignals() override { return mSignals; }

private:
  void synchronizeComponents(SystemView &view);

  void synchronizeTransforms(SystemView &view);

private:
  physx::PxShape *createShape(Entity entity,
                              const PhysicsGeometryDesc &geometryDesc,
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
};

} // namespace quoll
