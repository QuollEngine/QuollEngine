#include "liquid/core/Base.h"
#include "PhysicsSystem.h"

#include <PxConfig.h>
#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>

#include "PhysxMapping.h"

#include "liquid/core/EngineGlobals.h"

#ifdef LIQUID_PROFILER
static const bool RECORD_MEMORY_ALLOCATIONS = true;
#else
static const bool RECORD_MEMORY_ALLOCATIONS = false;
#endif

using namespace physx;

namespace liquid {

class PhysxSimulationEventCallback : public PxSimulationEventCallback {
public:
  /**
   * @brief Create simulation event callback
   *
   * @param eventSystem Event system
   */
  PhysxSimulationEventCallback(EventSystem &eventSystem)
      : mEventSystem(eventSystem) {}

  /**
   * @brief Event when constraint is broken
   *
   * @param constraints Constraints
   * @param count Number of constraints
   */
  void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) override {}

  /**
   * @brief Event when actors are awoken
   *
   * @param actors Actors
   * @param count Number of actors
   */
  void onWake(PxActor **actors, PxU32 count) override {}

  /**
   * @brief Event when actors are asleep
   *
   * @param actors Actors
   * @param count Number of actors
   */
  void onSleep(PxActor **actors, PxU32 count) override {}

  /**
   * @brief Event when actors are in contact
   *
   * @param pairHeader Contact pair header
   * @param pairs Contact pairs
   * @param nbPairs Number of pairs
   */
  void onContact(const PxContactPairHeader &pairHeader,
                 const PxContactPair *pairs, PxU32 nbPairs) override {
    auto *actor1 = pairHeader.actors[0];
    auto *actor2 = pairHeader.actors[1];

    Entity e1 =
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(actor1->userData));
    Entity e2 =
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(actor2->userData));

    for (PxU32 i = 0; i < nbPairs; ++i) {
      const PxContactPair &cp = pairs[i];

      if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
        mEventSystem.dispatch(CollisionEvent::CollisionStarted, {e1, e2});
      } else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST) {
        mEventSystem.dispatch(CollisionEvent::CollisionEnded, {e1, e2});
      }
    }
  }

  /**
   * @brief Event when actors are triggered
   *
   * @param pairs Trigger pairs
   * @param count Number of pairs
   */
  void onTrigger(PxTriggerPair *pairs, PxU32 count) override {}

  /**
   * @brief Event when simulation is running
   *
   * @param bodyBuffer Rigid body buffer
   * @param poseBuffer Pose buffer
   * @param count Number of items
   */
  void onAdvance(const PxRigidBody *const *bodyBuffer,
                 const PxTransform *poseBuffer, const PxU32 count) override {}

private:
  EventSystem &mEventSystem;
};

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

class PhysicsSystem::PhysicsSystemImpl {
public:
  /**
   * @brief Create physics system
   *
   * Initialize Physx
   *
   * @param eventSystem Event system
   */
  PhysicsSystemImpl(EventSystem &eventSystem)
      : mEventSystem(eventSystem), mSimulationEventCallback(mEventSystem) {
    constexpr uint32_t PVD_PORT = 5425;
    constexpr uint32_t PVD_TIMEOUT_IN_MS = 2000;
    constexpr glm::vec3 GRAVITY(0.0f, -9.8f, 0.0f);

    mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocator,
                                     mDefaultErrorCallback);

    mPvd = PxCreatePvd(*mFoundation);
    PxPvdTransport *transport = PxDefaultPvdSocketTransportCreate(
        "127.0.0.1", PVD_PORT, PVD_TIMEOUT_IN_MS);
    mPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

    mPhysics =
        PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(),
                        RECORD_MEMORY_ALLOCATIONS, mPvd);

    PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
    mDispatcher = PxDefaultCpuDispatcherCreate(1);
    sceneDesc.cpuDispatcher = mDispatcher;
    sceneDesc.filterShader = phyxFilterAllCollisionShader;
    sceneDesc.gravity = PxVec3(GRAVITY.x, GRAVITY.y, GRAVITY.z);
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

  PhysicsSystemImpl(const PhysicsSystemImpl &) = delete;
  PhysicsSystemImpl &operator=(const PhysicsSystemImpl &) = delete;
  PhysicsSystemImpl(PhysicsSystemImpl &&) = delete;
  PhysicsSystemImpl &operator=(PhysicsSystemImpl &&) = delete;

  /**
   * @brief Destroy physics system
   *
   * Destroy PhysX objects
   */
  ~PhysicsSystemImpl() {
    mScene->release();
    mDispatcher->release();
    mPhysics->release();
    mPvd->release();
    mFoundation->release();
  }

  /**
   * @brief Create Physx shape
   *
   * @param geometryDesc Geometry description
   * @param material Physx material
   * @return
   */
  PxShape *createShape(const PhysicsGeometryDesc &geometryDesc,
                       PxMaterial &material) {

    PxShape *shape = nullptr;
    if (geometryDesc.type == PhysicsGeometryType::Sphere) {
      const auto &[radius] =
          std::get<PhysicsGeometrySphere>(geometryDesc.params);
      const auto &geometry = PxSphereGeometry(radius);
      shape = mPhysics->createShape(geometry, material, true);
    }

    if (geometryDesc.type == PhysicsGeometryType::Box) {
      const auto &[halfExtents] =
          std::get<PhysicsGeometryBox>(geometryDesc.params);
      const auto &geometry =
          PxBoxGeometry(halfExtents.x, halfExtents.y, halfExtents.z);
      shape = mPhysics->createShape(geometry, material, true);
    } /**
       * @brief Send collision events
       */
    void sendCollisionEvents();

    if (geometryDesc.type == PhysicsGeometryType::Capsule) {
      const auto &[radius, halfHeight] =
          std::get<PhysicsGeometryCapsule>(geometryDesc.params);
      const auto &geometry = PxCapsuleGeometry(radius, halfHeight);
      shape = mPhysics->createShape(geometry, material, true);
    }

    if (geometryDesc.type == PhysicsGeometryType::Plane) {
      const auto &geometry = PxPlaneGeometry();
      shape = mPhysics->createShape(geometry, material, true);
    }

    return shape;
  }

  /**
   * @brief Update physx shape with geometry data
   *
   * @param geometryDesc Geometry description
   * @param shape Physx shape
   */
  void updateShapeWithGeometryData(const PhysicsGeometryDesc &geometryDesc,
                                   PxShape *shape) {
    if (geometryDesc.type == PhysicsGeometryType::Sphere) {
      const auto &[radius] =
          std::get<PhysicsGeometrySphere>(geometryDesc.params);
      const auto &geometry = PxSphereGeometry(radius);
      shape->setGeometry(geometry);
    } else if (geometryDesc.type == PhysicsGeometryType::Box) {
      const auto &[halfExtents] =
          std::get<PhysicsGeometryBox>(geometryDesc.params);
      const auto &geometry =
          PxBoxGeometry(halfExtents.x, halfExtents.y, halfExtents.z);
      shape->setGeometry(geometry);
    } else if (geometryDesc.type == PhysicsGeometryType::Capsule) {
      const auto &[radius, halfHeight] =
          std::get<PhysicsGeometryCapsule>(geometryDesc.params);
      const auto &geometry = PxCapsuleGeometry(radius, halfHeight);
      shape->setGeometry(geometry);
    } else if (geometryDesc.type == PhysicsGeometryType::Plane) {
      const auto &geometry = PxPlaneGeometry();
      shape->setGeometry(geometry);
    }
  }

  /**
   * @brief Get PhysX scene
   *
   * @return PhysX scene
   */
  inline PxScene *getScene() { return mScene; }

  /**
   * @brief Get PhysX physics SDK
   * @return PhysX physics SDK
   */
  inline PxPhysics *getPhysics() { return mPhysics; }

private:
  EventSystem &mEventSystem;

  PxDefaultAllocator mDefaultAllocator;
  PxDefaultErrorCallback mDefaultErrorCallback;
  PhysxSimulationEventCallback mSimulationEventCallback;

  PxPvd *mPvd = nullptr;
  PxFoundation *mFoundation = nullptr;
  PxPhysics *mPhysics = nullptr;
  PxDefaultCpuDispatcher *mDispatcher = nullptr;

  PxScene *mScene = nullptr;
};

PhysicsSystem::PhysicsSystem(EventSystem &eventSystem)
    : mEventSystem(eventSystem) {
  mImpl = new PhysicsSystemImpl(mEventSystem);
}

PhysicsSystem::~PhysicsSystem() { delete mImpl; }

void PhysicsSystem::update(float dt, EntityContext &entityContext) {
  LIQUID_PROFILE_EVENT("PhysicsSystem::update");

  synchronizeComponents(entityContext);

  mImpl->getScene()->simulate(dt);
  mImpl->getScene()->fetchResults(true);

  synchronizeTransforms(entityContext);
}

void PhysicsSystem::synchronizeComponents(EntityContext &entityContext) {
  LIQUID_PROFILE_EVENT("PhysicsSystem::synchronizeEntitiesWithPhysx");

  {
    LIQUID_PROFILE_EVENT("Cleanup unused static rigid bodies");
    PxActorTypeFlags desiredType = PxActorTypeFlag::eRIGID_STATIC;
    PxU32 count = mImpl->getScene()->getNbActors(desiredType);
    PxActor **buffer = new PxActor *[count];
    mImpl->getScene()->getActors(desiredType, buffer, count);
    for (PxU32 i = 0; i < count; ++i) {
      PxRigidStatic *actor = static_cast<PxRigidStatic *>(buffer[i]);

      Entity entity =
          static_cast<uint32_t>(reinterpret_cast<uintptr_t>(actor->userData));

      // Destroy actor and shape if there is
      // no collidable component
      if (!entityContext.hasComponent<CollidableComponent>(entity)) {
        physx::PxShape *shape = nullptr;
        actor->getShapes(&shape, 1);

        actor->detachShape(*shape);
        shape->release();

        mImpl->getScene()->removeActor(*actor);
      }
    }
    delete[] buffer;
  }

  {
    LIQUID_PROFILE_EVENT("Cleanup unused rigid bodies");
    PxActorTypeFlags desiredType = PxActorTypeFlag::eRIGID_DYNAMIC;
    PxU32 count = mImpl->getScene()->getNbActors(desiredType);
    PxActor **buffer = new PxActor *[count];
    mImpl->getScene()->getActors(desiredType, buffer, count);
    for (PxU32 i = 0; i < count; ++i) {
      PxRigidDynamic *actor = static_cast<PxRigidDynamic *>(buffer[i]);

      Entity entity =
          static_cast<uint32_t>(reinterpret_cast<uintptr_t>(actor->userData));

      // Destroy shape in actor if collidable component
      // is removed
      if (!entityContext.hasComponent<CollidableComponent>(entity) &&
          actor->getNbShapes() > 0) {
        PxShape *shape = nullptr;
        actor->getShapes(&shape, 1);

        actor->detachShape(*shape);
        shape->release();
      }

      // Destroy actor itself if rigid body component is removed
      if (!entityContext.hasComponent<RigidBodyComponent>(entity)) {
        mImpl->getScene()->removeActor(*actor);
        actor->release();
      }
    }
    delete[] buffer;
  }

  {
    LIQUID_PROFILE_EVENT("Synchronize collidable components");
    entityContext.iterateEntities<CollidableComponent, WorldTransformComponent>(
        [this, &entityContext](auto entity, CollidableComponent &collidable,
                               const WorldTransformComponent &world) {
          // Create or set material
          if (!collidable.material) {
            collidable.material = mImpl->getPhysics()->createMaterial(
                collidable.materialDesc.staticFriction,
                collidable.materialDesc.dynamicFriction,
                collidable.materialDesc.restitution);
          } else {
            collidable.material->setRestitution(
                collidable.materialDesc.restitution);
            collidable.material->setStaticFriction(
                collidable.materialDesc.staticFriction);
            collidable.material->setDynamicFriction(
                collidable.materialDesc.dynamicFriction);
          }

          // Create or set shape
          if (!collidable.shape) {
            collidable.shape = mImpl->createShape(collidable.geometryDesc,
                                                  *collidable.material);

            collidable.material->release();
          } else if (PhysxMapping::getPhysxGeometryType(
                         collidable.geometryDesc.type) ==
                     collidable.shape->getGeometryType()) {
            mImpl->updateShapeWithGeometryData(collidable.geometryDesc,
                                               collidable.shape);
          } else {
            auto *newShape = mImpl->createShape(collidable.geometryDesc,
                                                *collidable.material);

            if (entityContext.hasComponent<RigidBodyComponent>(entity)) {
              RigidBodyComponent &rigidBody =
                  entityContext.getComponent<RigidBodyComponent>(entity);
              rigidBody.actor->detachShape(*collidable.shape);
              rigidBody.actor->attachShape(*newShape);
            } else {
              collidable.rigidStatic->detachShape(*collidable.shape);
              collidable.rigidStatic->attachShape(*newShape);
            }

            collidable.shape->release();
            collidable.shape = newShape;
          }

          // Create rigid static if no rigid body
          if (!entityContext.hasComponent<RigidBodyComponent>(entity) &&
              !collidable.rigidStatic) {
            collidable.rigidStatic = mImpl->getPhysics()->createRigidStatic(
                PhysxMapping::getPhysxTransform(world.worldTransform));
            collidable.rigidStatic->attachShape(*collidable.shape);
            collidable.rigidStatic->userData =
                reinterpret_cast<void *>(static_cast<uintptr_t>(entity));

            mImpl->getScene()->addActor(*collidable.rigidStatic);
          } else if (collidable.rigidStatic) {
            // Update transform of rigid static if exists
            collidable.rigidStatic->setGlobalPose(
                PhysxMapping::getPhysxTransform(world.worldTransform));
          }
        });
  }

  {
    LIQUID_PROFILE_EVENT("Synchronize rigid body components");
    entityContext.iterateEntities<RigidBodyComponent, WorldTransformComponent>(
        [this, &entityContext](auto entity, RigidBodyComponent &rigidBody,
                               WorldTransformComponent &world) {
          if (!rigidBody.actor) {
            rigidBody.actor = mImpl->getPhysics()->createRigidDynamic(
                PhysxMapping::getPhysxTransform(world.worldTransform));
            rigidBody.actor->userData =
                reinterpret_cast<void *>(static_cast<uintptr_t>(entity));

            mImpl->getScene()->addActor(*rigidBody.actor);

            // Clear collidable's rigid body if exists
            if (entityContext.hasComponent<CollidableComponent>(entity)) {
              CollidableComponent &collidable =
                  entityContext.getComponent<CollidableComponent>(entity);
              if (collidable.rigidStatic) {
                mImpl->getScene()->removeActor(*collidable.rigidStatic, false);
                collidable.rigidStatic->release();
                collidable.rigidStatic = nullptr;
              }
            }
          }

          if (entityContext.hasComponent<CollidableComponent>(entity) &&
              rigidBody.actor->getNbShapes() == 0) {
            rigidBody.actor->attachShape(
                *entityContext.getComponent<CollidableComponent>(entity).shape);
          }

          rigidBody.actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY,
                                        !rigidBody.dynamicDesc.applyGravity);
          rigidBody.actor->setMass(rigidBody.dynamicDesc.mass);
          rigidBody.actor->setMassSpaceInertiaTensor(
              {rigidBody.dynamicDesc.inertia.x, rigidBody.dynamicDesc.inertia.y,
               rigidBody.dynamicDesc.inertia.z});
          rigidBody.actor->setGlobalPose(
              PhysxMapping::getPhysxTransform(world.worldTransform));
        });
  }
}

void PhysicsSystem::synchronizeTransforms(EntityContext &entityContext) {
  LIQUID_PROFILE_EVENT("PhysicsSystem::synchronizeTransforms");
  uint32_t count = 0;
  auto **actors = mImpl->getScene()->getActiveActors(count);

  {
    LIQUID_PROFILE_EVENT("Synchronize world transforms");
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
          static_cast<uint32_t>(reinterpret_cast<uintptr_t>(actor->userData));

      glm::vec3 position(globalTransform.p.x, globalTransform.p.y,
                         globalTransform.p.z);
      glm::quat rotation(globalTransform.q.w, globalTransform.q.x,
                         globalTransform.q.y, globalTransform.q.z);

      if (entityContext.hasComponent<WorldTransformComponent>(entity)) {
        auto &world =
            entityContext.getComponent<WorldTransformComponent>(entity);

        glm::quat emptyQuat;
        glm::vec3 scale;
        glm::vec3 empty3;
        glm::vec4 empty4;

        glm::decompose(world.worldTransform, scale, emptyQuat, empty3, empty3,
                       empty4);

        world.worldTransform = glm::translate(glm::mat4{1.0f}, position) *
                               glm::toMat4(rotation) *
                               glm::scale(glm::mat4{1.0f}, scale);
      }
    }
  }

  {
    LIQUID_PROFILE_EVENT("Synchronize local transforms");
    // Calculate local transforms from parent world transforms
    for (uint32_t i = 0; i < count; ++i) {
      if (actors[i]->getType() != PxActorType::eRIGID_DYNAMIC) {
        continue;
      }
      auto *actor = static_cast<PxRigidActor *>(actors[i]);
      const auto &globalTransform = actor->getGlobalPose();

      Entity entity =
          static_cast<uint32_t>(reinterpret_cast<uintptr_t>(actor->userData));

      glm::vec3 position(globalTransform.p.x, globalTransform.p.y,
                         globalTransform.p.z);
      glm::quat rotation(globalTransform.q.w, globalTransform.q.x,
                         globalTransform.q.y, globalTransform.q.z);

      if (entityContext.hasComponent<LocalTransformComponent>(entity)) {
        auto &transform =
            entityContext.getComponent<LocalTransformComponent>(entity);

        if (entityContext.hasComponent<ParentComponent>(entity)) {
          const auto &parentTransform =
              entityContext.getComponent<WorldTransformComponent>(
                  entityContext.getComponent<ParentComponent>(entity).parent);

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

} // namespace liquid
