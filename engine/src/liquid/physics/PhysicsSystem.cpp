#include "liquid/core/Base.h"
#include "PhysicsSystem.h"

#include <PxConfig.h>
#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>

#include "liquid/core/EngineGlobals.h"

#ifdef LIQUID_PROFILER
static const bool RECORD_MEMORY_ALLOCATIONS = true;
#else
static const bool RECORD_MEMORY_ALLOCATIONS = false;
#endif

namespace liquid {

class PhysicsSystem::PhysicsSystemImpl {
public:
  /**
   * @brief Create physics system
   *
   * Initialize PhysX
   */
  PhysicsSystemImpl() {
    constexpr uint32_t PVD_PORT = 5425;
    constexpr uint32_t PVD_TIMEOUT_IN_MS = 2000;
    constexpr glm::vec3 GRAVITY(0.0f, -9.8f, 0.0f);

    foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocator,
                                    defaultErrorCallback);

    pvd = physx::PxCreatePvd(*foundation);
    physx::PxPvdTransport *transport = physx::PxDefaultPvdSocketTransportCreate(
        "127.0.0.1", PVD_PORT, PVD_TIMEOUT_IN_MS);
    pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
                              physx::PxTolerancesScale(),
                              RECORD_MEMORY_ALLOCATIONS, pvd);

    physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
    dispatcher = physx::PxDefaultCpuDispatcherCreate(1);
    sceneDesc.cpuDispatcher = dispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    sceneDesc.gravity = physx::PxVec3(GRAVITY.x, GRAVITY.y, GRAVITY.z);
    sceneDesc.flags = physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    scene = physics->createScene(sceneDesc);

    auto *pvdClient = scene->getScenePvdClient();
    if (pvdClient) {
      pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS,
                                 true);
      pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS,
                                 true);
      pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES,
                                 true);
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
    scene->release();
    dispatcher->release();
    physics->release();
    pvd->release();
    foundation->release();
  }

  /**
   * @brief Get PhysX scene
   *
   * @return PhysX scene
   */
  inline physx::PxScene *getScene() { return scene; }

  /**
   * @brief Get PhysX physics SDK
   * @return PhysX physics SDK
   */
  inline physx::PxPhysics *getPhysics() { return physics; }

private:
  physx::PxDefaultAllocator defaultAllocator;
  physx::PxDefaultErrorCallback defaultErrorCallback;

  physx::PxPvd *pvd = nullptr;
  physx::PxFoundation *foundation = nullptr;
  physx::PxPhysics *physics = nullptr;
  physx::PxDefaultCpuDispatcher *dispatcher = nullptr;

  physx::PxScene *scene = nullptr;
};

PhysicsSystem::PhysicsSystem(EntityContext &entityContext_)
    : entityContext(entityContext_) {
  impl = new PhysicsSystemImpl;
}

PhysicsSystem::~PhysicsSystem() { delete impl; }

void PhysicsSystem::update(float dt) {
  LIQUID_PROFILE_EVENT("PhysicsSystem::update");

  glm::vec3 empty3;
  glm::vec4 empty4;

  entityContext.iterateEntities<TransformComponent, CollidableComponent>(
      [&empty3, &empty4](auto entity, const TransformComponent &transform,
                         CollidableComponent &collidable) {
        if (!collidable.rigidStatic)
          return;

        glm::vec3 position;
        glm::quat rotation;

        glm::decompose(transform.worldTransform, empty3, rotation, position,
                       empty3, empty4);

        physx::PxTransform pose;
        pose.p.x = position.x;
        pose.p.y = position.y;
        pose.p.z = position.z;

        pose.q.w = rotation.w;
        pose.q.x = rotation.x;
        pose.q.y = rotation.y;
        pose.q.z = rotation.z;

        collidable.rigidStatic->setGlobalPose(pose);
      });

  // sync transforms with physics system
  entityContext.iterateEntities<TransformComponent, RigidBodyComponent>(
      [&empty3, &empty4](auto entity, const TransformComponent &transform,
                         RigidBodyComponent &rigidBody) {
        glm::vec3 position;
        glm::quat rotation;

        glm::decompose(transform.worldTransform, empty3, rotation, position,
                       empty3, empty4);

        physx::PxTransform pose;
        pose.p.x = position.x;
        pose.p.y = position.y;
        pose.p.z = position.z;

        pose.q.w = rotation.w;
        pose.q.x = rotation.x;
        pose.q.y = rotation.y;
        pose.q.z = rotation.z;

        rigidBody.actor->setGlobalPose(pose);
      });

  impl->getScene()->simulate(dt);
  impl->getScene()->fetchResults(true);

  uint32_t count = 0;
  auto **actors = impl->getScene()->getActiveActors(count);

  // calculate world transforms first so that local
  // transforms are calculated from the most recent
  // values
  for (uint32_t i = 0; i < count; ++i) {
    if (actors[i]->getType() != physx::PxActorType::eRIGID_STATIC &&
        actors[i]->getType() != physx::PxActorType::eRIGID_DYNAMIC) {
      continue;
    }
    auto *actor = static_cast<physx::PxRigidActor *>(actors[i]);
    const auto &globalTransform = actor->getGlobalPose();

    Entity entity =
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(actor->userData));

    glm::vec3 position(globalTransform.p.x, globalTransform.p.y,
                       globalTransform.p.z);
    glm::quat rotation(globalTransform.q.w, globalTransform.q.x,
                       globalTransform.q.y, globalTransform.q.z);

    if (entityContext.hasComponent<TransformComponent>(entity)) {
      auto &transform = entityContext.getComponent<TransformComponent>(entity);

      glm::quat emptyQuat;
      glm::vec3 scale;

      glm::decompose(transform.worldTransform, scale, emptyQuat, empty3, empty3,
                     empty4);

      transform.worldTransform = glm::translate(glm::mat4{1.0f}, position) *
                                 glm::toMat4(rotation) *
                                 glm::scale(glm::mat4{1.0f}, scale);
    }
  }

  // Calculate local transforms from parent world transforms
  for (uint32_t i = 0; i < count; ++i) {
    if (actors[i]->getType() != physx::PxActorType::eRIGID_STATIC &&
        actors[i]->getType() != physx::PxActorType::eRIGID_DYNAMIC) {
      continue;
    }
    auto *actor = static_cast<physx::PxRigidActor *>(actors[i]);
    const auto &globalTransform = actor->getGlobalPose();

    Entity entity =
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(actor->userData));

    glm::vec3 position(globalTransform.p.x, globalTransform.p.y,
                       globalTransform.p.z);
    glm::quat rotation(globalTransform.q.w, globalTransform.q.x,
                       globalTransform.q.y, globalTransform.q.z);

    if (entityContext.hasComponent<TransformComponent>(entity)) {
      auto &transform = entityContext.getComponent<TransformComponent>(entity);

      if (entityContext.hasComponent<TransformComponent>(transform.parent)) {
        const auto &parentTransform =
            entityContext.getComponent<TransformComponent>(transform.parent);

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

void PhysicsSystem::createCollidableComponent(
    liquid::Entity entity, const PhysicsMaterialDesc &materialDesc,
    const PhysicsGeometryDesc &geometryDesc) {
  if (!entityContext.hasComponent<TransformComponent>(entity)) {
    return;
  }

  auto *material = impl->getPhysics()->createMaterial(
      materialDesc.staticFriction, materialDesc.dynamicFriction,
      materialDesc.restitution);

  physx::PxShape *shape = nullptr;

  if (geometryDesc.type == PhysicsGeometryType::Sphere) {
    const auto &[radius] = std::get<PhysicsGeometrySphere>(geometryDesc.params);
    const auto &geometry = physx::PxSphereGeometry(radius);
    shape = impl->getPhysics()->createShape(geometry, *material);
  } else if (geometryDesc.type == PhysicsGeometryType::Box) {
    const auto &[halfExtents] =
        std::get<PhysicsGeometryBox>(geometryDesc.params);
    const auto &geometry =
        physx::PxBoxGeometry(halfExtents.x, halfExtents.y, halfExtents.z);
    shape = impl->getPhysics()->createShape(geometry, *material);
  } else if (geometryDesc.type == PhysicsGeometryType::Capsule) {
    const auto &[radius, halfHeight] =
        std::get<PhysicsGeometryCapsule>(geometryDesc.params);
    const auto &geometry = physx::PxCapsuleGeometry(radius, halfHeight);
    shape = impl->getPhysics()->createShape(geometry, *material);
  } else if (geometryDesc.type == PhysicsGeometryType::Plane) {
    const auto &geometry = physx::PxPlaneGeometry();
    shape = impl->getPhysics()->createShape(geometry, *material);
  }

  if (!shape) {
    engineLogger.log(Logger::Warning)
        << "Invalid shape parameters. Shape was not created";
    return;
  }

  CollidableComponent component;
  component.material = material;
  component.shape = shape;

  if (entityContext.hasComponent<RigidBodyComponent>(entity)) {
    entityContext.getComponent<RigidBodyComponent>(entity).actor->attachShape(
        *shape);
  } else {
    auto *rigidBody = impl->getPhysics()->createRigidStatic(
        physx::PxTransform(physx::PxVec3{0.0f, 0.0f, 0.0f},
                           physx::PxQuat{0.0f, 0.0f, 0.0f, 1.0f}));

    rigidBody->attachShape(*shape);
    rigidBody->userData =
        reinterpret_cast<void *>(static_cast<uintptr_t>(entity));

    impl->getScene()->addActor(*rigidBody);
    component.rigidStatic = rigidBody;
  }

  entityContext.setComponent(entity, component);
}

void PhysicsSystem::createRigidBodyComponent(
    liquid::Entity entity, const PhysicsDynamicRigidBodyDesc &dynamicDesc) {
  if (!entityContext.hasComponent<TransformComponent>(entity)) {
    return;
  }

  physx::PxTransform transform({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f});
  physx::PxShape *shape = nullptr;
  if (entityContext.hasComponent<CollidableComponent>(entity)) {
    auto &collidable = entityContext.getComponent<CollidableComponent>(entity);
    if (collidable.rigidStatic) {
      transform = collidable.rigidStatic->getGlobalPose();
      impl->getScene()->removeActor(*collidable.rigidStatic, false);
      collidable.rigidStatic->release();
      collidable.rigidStatic = nullptr;
    }
    shape = collidable.shape;
  }

  auto *rigidBody = impl->getPhysics()->createRigidDynamic(transform);
  rigidBody->userData =
      reinterpret_cast<void *>(static_cast<uintptr_t>(entity));

  rigidBody->setMass(dynamicDesc.mass);
  rigidBody->setMassSpaceInertiaTensor(
      {dynamicDesc.inertia.x, dynamicDesc.inertia.y, dynamicDesc.inertia.z});

  if (shape) {
    rigidBody->attachShape(*shape);
  }

  impl->getScene()->addActor(*rigidBody);

  RigidBodyComponent component;
  component.actor = rigidBody;

  entityContext.setComponent(entity, component);
}

} // namespace liquid
