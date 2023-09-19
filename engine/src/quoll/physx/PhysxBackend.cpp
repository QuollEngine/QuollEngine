#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "PhysxBackend.h"
#include "PhysxMapping.h"

#ifdef QUOLL_PROFILER
static const bool RECORD_MEMORY_ALLOCATIONS = true;
#else
static const bool RECORD_MEMORY_ALLOCATIONS = false;
#endif

using namespace physx;

namespace quoll {

/**
 * @brief Update physx shape with geometry data
 *
 * @param geometryDesc Geometry description
 * @param shape Physx shape
 * @param worldTransform World transform
 */
static void updateShapeWithGeometryData(const PhysicsGeometryDesc &geometryDesc,
                                        PxShape *shape,
                                        const glm::mat4 &worldTransform) {
  glm::vec3 scale;
  glm::quat rotation;
  glm::vec3 translation;
  glm::vec3 skew;
  glm::vec4 perspective;
  glm::decompose(worldTransform, scale, rotation, translation, skew,
                 perspective);

  scale = glm::abs(scale);

  if (geometryDesc.type == PhysicsGeometryType::Sphere) {
    const auto &[radius] = std::get<PhysicsGeometrySphere>(geometryDesc.params);
    PxSphereGeometry geometry(std::max(scale.x, std::max(scale.y, scale.z)) *
                              radius);
    shape->setGeometry(geometry);
  } else if (geometryDesc.type == PhysicsGeometryType::Box) {
    const auto &[halfExtents] =
        std::get<PhysicsGeometryBox>(geometryDesc.params);
    PxBoxGeometry geometry(scale.x * halfExtents.x, scale.y * halfExtents.y,
                           scale.z * halfExtents.z);
    shape->setGeometry(geometry);
  } else if (geometryDesc.type == PhysicsGeometryType::Capsule) {
    const auto &[radius, halfHeight] =
        std::get<PhysicsGeometryCapsule>(geometryDesc.params);
    PxCapsuleGeometry geometry(scale.x * radius, scale.y * halfHeight);
    shape->setGeometry(geometry);

    PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
    shape->setLocalPose(relativePose);
  } else if (geometryDesc.type == PhysicsGeometryType::Plane) {
    shape->setGeometry(PxPlaneGeometry());
  }
}

/**
 * @brief Shader that notifies on any collision
 */
static PxFilterFlags phyxFilterAllCollisionShader(
    PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags &pairFlags, const void *constantBlock,
    PxU32 constantBlockSize) {

  if (PxFilterObjectIsTrigger(attributes0) ||
      PxFilterObjectIsTrigger(attributes1)) {
    pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
    return PxFilterFlag::eDEFAULT;
  }

  pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND |
              PxPairFlag::eNOTIFY_TOUCH_LOST;

  return PxFilterFlag::eDEFAULT;
}

PhysxBackend::PhysxBackend(EventSystem &eventSystem)
    : mSimulationEventCallback(eventSystem) {
  static constexpr uint32_t PvdPort = 5425;
  static constexpr uint32_t PvdTimeoutInMs = 2000;
  static constexpr glm::vec3 Gravity(0.0f, -9.8f, 0.0f);

  mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocator,
                                   mDefaultErrorCallback);

  mPvd = PxCreatePvd(*mFoundation);
  PxPvdTransport *transport =
      PxDefaultPvdSocketTransportCreate("127.0.0.1", PvdPort, PvdTimeoutInMs);
  mPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

  Engine::getLogger().info()
      << "PhysX initialized. Visual debugger connected connected to "
         "127.0.0.1:"
      << PvdPort;

  mPhysics =
      PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(),
                      RECORD_MEMORY_ALLOCATIONS, mPvd);

  mDispatcher = PxDefaultCpuDispatcherCreate(1);

  PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
  sceneDesc.cpuDispatcher = mDispatcher;
  sceneDesc.filterShader = phyxFilterAllCollisionShader;
  sceneDesc.gravity = PxVec3(Gravity.x, Gravity.y, Gravity.z);
  sceneDesc.flags = PxSceneFlag::eENABLE_ACTIVE_ACTORS;
  sceneDesc.simulationEventCallback = &mSimulationEventCallback;
  mScene = mPhysics->createScene(sceneDesc);

  auto *pvdClient = mScene->getScenePvdClient();
  if (pvdClient) {
    pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
    pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
    pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
  }
}

PhysxBackend::~PhysxBackend() {
  mScene->release();
  mDispatcher->release();
  mPhysics->release();
  mPvd->release();
  mFoundation->release();
}

void PhysxBackend::update(float dt, EntityDatabase &entityDatabase) {
  QUOLL_PROFILE_EVENT("PhysicsSystem::update");

  synchronizeComponents(entityDatabase);

  mScene->simulate(dt);
  mScene->fetchResults(true);

  synchronizeTransforms(entityDatabase);
}

void PhysxBackend::cleanup(EntityDatabase &entityDatabase) {
  for (auto [entity, physx] : entityDatabase.view<PhysxInstance>()) {
    if (physx.rigidStatic) {
      if (physx.shape) {
        physx.rigidStatic->detachShape(*physx.shape);
      }
      mScene->removeActor(*physx.rigidStatic);
      physx.rigidStatic->release();
    }

    if (physx.rigidDynamic) {
      if (physx.shape) {
        physx.rigidDynamic->detachShape(*physx.shape);
      }
      mScene->removeActor(*physx.rigidDynamic);
      physx.rigidDynamic->release();
    }
  }

  entityDatabase.destroyComponents<PhysxInstance>();
  mPhysxInstanceRemoveObserver.clear();
}

void PhysxBackend::observeChanges(EntityDatabase &entityDatabase) {
  mPhysxInstanceRemoveObserver = entityDatabase.observeRemove<PhysxInstance>();
}

bool PhysxBackend::sweep(EntityDatabase &entityDatabase, Entity entity,
                         const glm::vec3 &direction, float distance) {
  QuollAssert(entityDatabase.has<PhysxInstance>(entity),
              "Physx instance not found");

  const auto &physx = entityDatabase.get<PhysxInstance>(entity);
  const auto &transform = entityDatabase.get<WorldTransform>(entity);

  PxSweepBuffer hit;

  return mScene->sweep(
      physx.shape->getGeometry().any(),
      PhysxMapping::getPhysxTransform(transform.worldTransform),
      PhysxMapping::getPhysxVec3(direction), distance, hit);
}

void PhysxBackend::synchronizeComponents(EntityDatabase &entityDatabase) {
  QUOLL_PROFILE_EVENT("PhysicsSystem::synchronizeEntitiesWithPhysx");

  {
    QUOLL_PROFILE_EVENT("Cleanup dangling physx objects in scene");
    for (auto [entity, physx] : mPhysxInstanceRemoveObserver) {
      if (physx.rigidDynamic) {
        mScene->removeActor(*physx.rigidDynamic);
        physx.rigidDynamic->release();
      }

      if (physx.rigidStatic) {
        mScene->removeActor(*physx.rigidStatic);
        physx.rigidStatic->release();
      }

      if (physx.material) {
        physx.material->release();
      }
    }

    mPhysxInstanceRemoveObserver.clear();
  }

  {
    QUOLL_PROFILE_EVENT("Synchronize collidable components");
    for (auto [entity, collidable, world] :
         entityDatabase.view<Collidable, WorldTransform>()) {
      if (!entityDatabase.has<PhysxInstance>(entity)) {
        entityDatabase.set<PhysxInstance>(entity, {});
      }
      auto &physx = entityDatabase.get<PhysxInstance>(entity);

      // Create or set material
      if (!physx.material) {
        physx.material =
            mPhysics->createMaterial(collidable.materialDesc.staticFriction,
                                     collidable.materialDesc.dynamicFriction,
                                     collidable.materialDesc.restitution);
      } else {
        physx.material->setRestitution(collidable.materialDesc.restitution);
        physx.material->setStaticFriction(
            collidable.materialDesc.staticFriction);
        physx.material->setDynamicFriction(
            collidable.materialDesc.dynamicFriction);
      }

      // Create or set shape
      if (!physx.shape) {
        physx.shape = createShape(collidable.geometryDesc, *physx.material,
                                  world.worldTransform);

        physx.material->release();
      } else if (PhysxMapping::getPhysxGeometryType(
                     collidable.geometryDesc.type) ==
                 physx.shape->getGeometryType()) {
        updateShapeWithGeometryData(collidable.geometryDesc, physx.shape,
                                    world.worldTransform);
      } else {
        auto *newShape = createShape(collidable.geometryDesc, *physx.material,
                                     world.worldTransform);

        if (entityDatabase.has<RigidBody>(entity)) {
          physx.rigidDynamic->detachShape(*physx.shape);
          physx.rigidDynamic->attachShape(*newShape);
        } else {
          physx.rigidStatic->detachShape(*physx.shape);
          physx.rigidStatic->attachShape(*newShape);
        }

        physx.shape->release();
        physx.shape = newShape;
      }

      if (physx.useShapeInSimulation != collidable.useInSimulation) {
        physx.shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE,
                             collidable.useInSimulation);
      }
      physx.useShapeInSimulation = collidable.useInSimulation;

      if (physx.useShapeInQueries != collidable.useInQueries) {
        physx.shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE,
                             collidable.useInQueries);
      }
      physx.useShapeInQueries = collidable.useInQueries;

      // Create rigid static if no rigid body
      if (!entityDatabase.has<RigidBody>(entity) && !physx.rigidStatic) {
        physx.rigidStatic = mPhysics->createRigidStatic(
            PhysxMapping::getPhysxTransform(world.worldTransform));
        physx.rigidStatic->attachShape(*physx.shape);
        physx.rigidStatic->userData =
            reinterpret_cast<void *>(static_cast<uintptr_t>(entity));

        mScene->addActor(*physx.rigidStatic);
      } else if (physx.rigidStatic) {
        // Update transform of rigid static if exists
        auto transform = glm::translate(world.worldTransform,
                                        collidable.geometryDesc.center);
        physx.rigidStatic->setGlobalPose(
            PhysxMapping::getPhysxTransform(transform));
      }
    }
  }

  {
    QUOLL_PROFILE_EVENT("Synchronize rigid body components");
    for (auto [entity, rigidBody, world] :
         entityDatabase.view<RigidBody, WorldTransform>()) {
      if (!entityDatabase.has<PhysxInstance>(entity)) {
        entityDatabase.set<PhysxInstance>(entity, {});
      }

      auto &physx = entityDatabase.get<PhysxInstance>(entity);

      if (!physx.rigidDynamic) {
        physx.rigidDynamic = mPhysics->createRigidDynamic(
            PhysxMapping::getPhysxTransform(world.worldTransform));
        physx.rigidDynamic->userData =
            reinterpret_cast<void *>(static_cast<uintptr_t>(entity));

        mScene->addActor(*physx.rigidDynamic);

        // Remove rigid static if exists
        if (physx.rigidStatic) {
          physx.rigidStatic->detachShape(*physx.shape);
          mScene->removeActor(*physx.rigidStatic, false);
          physx.rigidStatic->release();
          physx.rigidStatic = nullptr;
        }
      }

      if (physx.shape && physx.rigidDynamic->getNbShapes() == 0) {
        physx.rigidDynamic->attachShape(*physx.shape);
      }

      physx.rigidDynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY,
                                       !rigidBody.dynamicDesc.applyGravity);
      physx.rigidDynamic->setMass(rigidBody.dynamicDesc.mass);
      physx.rigidDynamic->setMassSpaceInertiaTensor(
          {rigidBody.dynamicDesc.inertia.x, rigidBody.dynamicDesc.inertia.y,
           rigidBody.dynamicDesc.inertia.z});

      if (entityDatabase.has<Collidable>(entity)) {
        auto transform = glm::translate(
            world.worldTransform,
            entityDatabase.get<Collidable>(entity).geometryDesc.center);

        physx.rigidDynamic->setGlobalPose(
            PhysxMapping::getPhysxTransform(transform));
      } else {
        physx.rigidDynamic->setGlobalPose(
            PhysxMapping::getPhysxTransform(world.worldTransform));
      }
    };
  }

  {
    QUOLL_PROFILE_EVENT("Clear rigid body velocities");
    for (auto [entity, _, _2, physx] :
         entityDatabase.view<RigidBodyClear, RigidBody, PhysxInstance>()) {
      physx.rigidDynamic->setLinearVelocity(PxVec3(0.0f));
      physx.rigidDynamic->setAngularVelocity(PxVec3(0.0f));
    }

    entityDatabase.destroyComponents<RigidBodyClear>();
  }

  {
    QUOLL_PROFILE_EVENT("Apply forces");

    for (auto [entity, force, _, physx] :
         entityDatabase.view<Force, RigidBody, PhysxInstance>()) {
      physx.rigidDynamic->addForce(
          PxVec3(force.force.x, force.force.y, force.force.z));
    };

    entityDatabase.destroyComponents<Force>();
  }

  {
    QUOLL_PROFILE_EVENT("Apply torques");

    for (auto [entity, torque, _, physx] :
         entityDatabase.view<Torque, RigidBody, PhysxInstance>()) {
      physx.rigidDynamic->addTorque(
          PxVec3(torque.torque.x, torque.torque.y, torque.torque.z));
    };

    entityDatabase.destroyComponents<Torque>();
  }
}

void PhysxBackend::synchronizeTransforms(EntityDatabase &entityDatabase) {
  QUOLL_PROFILE_EVENT("PhysicsSystem::synchronizeTransforms");
  uint32_t count = 0;
  auto **actors = mScene->getActiveActors(count);

  {
    QUOLL_PROFILE_EVENT("Synchronize world transforms");
    // calculate world transforms first so that local
    // transforms are calculated from the most recent
    // values
    for (uint32_t i = 0; i < count; ++i) {
      if (actors[i]->getType() != PxActorType::eRIGID_DYNAMIC) {
        continue;
      }
      auto *actor = static_cast<PxRigidActor *>(actors[i]);
      const auto &globalTransform = actor->getGlobalPose();

      Entity entity =
          static_cast<Entity>(reinterpret_cast<uintptr_t>(actor->userData));

      glm::vec3 position(globalTransform.p.x, globalTransform.p.y,
                         globalTransform.p.z);
      glm::quat rotation(globalTransform.q.w, globalTransform.q.x,
                         globalTransform.q.y, globalTransform.q.z);

      if (entityDatabase.has<WorldTransform>(entity)) {
        auto &world = entityDatabase.get<WorldTransform>(entity);

        glm::quat emptyQuat;
        glm::vec3 scale;
        glm::vec3 empty3;
        glm::vec4 empty4;

        glm::decompose(world.worldTransform, scale, emptyQuat, empty3, empty3,
                       empty4);

        if (entityDatabase.has<Collidable>(entity)) {
          position -=
              entityDatabase.get<Collidable>(entity).geometryDesc.center;
        }

        world.worldTransform = glm::translate(glm::mat4{1.0f}, position) *
                               glm::toMat4(rotation) *
                               glm::scale(glm::mat4{1.0f}, scale);
      }
    }
  }

  {
    QUOLL_PROFILE_EVENT("Synchronize local transforms");
    // Calculate local transforms from parent world transforms
    for (uint32_t i = 0; i < count; ++i) {
      if (actors[i]->getType() != PxActorType::eRIGID_DYNAMIC) {
        continue;
      }
      auto *actor = static_cast<PxRigidActor *>(actors[i]);
      const auto &globalTransform = actor->getGlobalPose();

      Entity entity =
          static_cast<Entity>(reinterpret_cast<uintptr_t>(actor->userData));

      glm::vec3 position(globalTransform.p.x, globalTransform.p.y,
                         globalTransform.p.z);
      glm::quat rotation(globalTransform.q.w, globalTransform.q.x,
                         globalTransform.q.y, globalTransform.q.z);

      if (entityDatabase.has<Collidable>(entity)) {
        position -= entityDatabase.get<Collidable>(entity).geometryDesc.center;
      }

      if (entityDatabase.has<LocalTransform>(entity)) {
        auto &transform = entityDatabase.get<LocalTransform>(entity);

        if (entityDatabase.has<Parent>(entity)) {
          const auto &parentTransform = entityDatabase.get<WorldTransform>(
              entityDatabase.get<Parent>(entity).parent);

          const auto &invParentTransform =
              glm::inverse(parentTransform.worldTransform);

          transform.localPosition =
              glm::vec3(invParentTransform * glm::vec4(position, 1.0));
          transform.localRotation =
              glm::toQuat(invParentTransform * glm::toMat4(rotation));
        } else {
          transform.localPosition = position;
          transform.localRotation = rotation;
        }
      }
    }
  }
}

PxShape *PhysxBackend::createShape(const PhysicsGeometryDesc &geometryDesc,
                                   PxMaterial &material,
                                   const glm::mat4 &worldTransform) {
  glm::vec3 scale;
  glm::quat rotation;
  glm::vec3 translation;
  glm::vec3 skew;
  glm::vec4 perspective;
  glm::decompose(worldTransform, scale, rotation, translation, skew,
                 perspective);

  scale = glm::abs(scale);

  PxShape *shape = nullptr;
  if (geometryDesc.type == PhysicsGeometryType::Sphere) {
    const auto &[radius] = std::get<PhysicsGeometrySphere>(geometryDesc.params);
    PxSphereGeometry geometry(std::max(scale.x, std::max(scale.y, scale.z)) *
                              radius);
    shape = mPhysics->createShape(geometry, material, true);
  } else if (geometryDesc.type == PhysicsGeometryType::Box) {
    const auto &[halfExtents] =
        std::get<PhysicsGeometryBox>(geometryDesc.params);
    PxBoxGeometry geometry(scale.x * halfExtents.x, scale.y * halfExtents.y,
                           scale.z * halfExtents.z);
    shape = mPhysics->createShape(geometry, material, true);
  } else if (geometryDesc.type == PhysicsGeometryType::Capsule) {
    const auto &[radius, halfHeight] =
        std::get<PhysicsGeometryCapsule>(geometryDesc.params);
    PxCapsuleGeometry geometry(std::max(scale.x, scale.y) * radius,
                               scale.y * halfHeight);
    shape = mPhysics->createShape(geometry, material, true);

    PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
    shape->setLocalPose(relativePose);
  } else if (geometryDesc.type == PhysicsGeometryType::Plane) {
    shape = mPhysics->createShape(PxPlaneGeometry(), material, true);
  }

  return shape;
}

} // namespace quoll
