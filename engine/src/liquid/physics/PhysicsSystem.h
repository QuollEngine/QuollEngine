#pragma once

#include "liquid/entity/EntityContext.h"

namespace liquid {

struct PhysicsMaterialDesc {
  float staticFriction = 1.0f;
  float dynamicFriction = 1.0f;
  float restitution = 1.0f;
};

enum class PhysicsGeometryType { Sphere, Plane, Capsule, Box };

struct PhysicsGeometrySphere {
  float radius = 1.0f;
};

struct PhysicsGeometryPlane {};

struct PhysicsGeometryCapsule {
  float radius = 1.0f;
  float halfHeight = 1.0f;
};

struct PhysicsGeometryBox {
  glm::vec3 halfExtents;
};

struct PhysicsGeometryDesc {
  PhysicsGeometryType type = PhysicsGeometryType::Box;
  std::variant<PhysicsGeometrySphere, PhysicsGeometryPlane,
               PhysicsGeometryCapsule, PhysicsGeometryBox>
      params = PhysicsGeometryBox{{0.0f, 0.0f, 0.0f}};
};

struct PhysicsDynamicRigidBodyDesc {
  float mass = 1.0f;
  glm::vec3 inertia;
};

class PhysicsSystem {
  class PhysicsSystemImpl;

public:
  /**
   * @brief Create physics system
   *
   * @param entityContext Entity context
   */
  PhysicsSystem(EntityContext &entityContext);

  /**
   * @brief Destroy physics system
   */
  ~PhysicsSystem();

  PhysicsSystem(const PhysicsSystem &) = delete;
  PhysicsSystem &operator=(const PhysicsSystem &) = delete;
  PhysicsSystem(PhysicsSystem &&) = delete;
  PhysicsSystem &operator=(PhysicsSystem &&) = delete;

  /**
   * @brief Update physics
   *
   * Performs physics simulation
   *
   * @param dt Time delta
   */
  void update(float dt);

  /**
   * @brief Create collidable component
   *
   * @param entity Entity
   * @param materialDesc Material description
   * @param geometryDesc Geometry description
   */
  void createCollidableComponent(liquid::Entity entity,
                                 const PhysicsMaterialDesc &materialDesc,
                                 const PhysicsGeometryDesc &geometryDesc);

  /**
   * @brief Create rigid body component
   *
   * @param entity Entity
   * @param dynamicDesc Dynamic description
   */
  void createRigidBodyComponent(liquid::Entity entity,
                                const PhysicsDynamicRigidBodyDesc &dynamicDesc);

private:
  PhysicsSystemImpl *impl;
  EntityContext &entityContext;
};

} // namespace liquid
