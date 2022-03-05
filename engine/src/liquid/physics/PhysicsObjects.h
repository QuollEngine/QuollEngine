#pragma once

namespace liquid {

/**
 * @brief Describes material attached to a shape
 */
struct PhysicsMaterialDesc {
  float staticFriction = 1.0f;
  float dynamicFriction = 1.0f;
  float restitution = 1.0f;
};

/**
 * @brief Geometry types
 */
enum class PhysicsGeometryType { Sphere, Plane, Capsule, Box };

/**
 * @brief Sphere geometry data
 */
struct PhysicsGeometrySphere {
  float radius = 1.0f;
};

/**
 * @brief Plane geometry data
 *
 * Planes do not have any data; they are only used
 * for representation
 */
struct PhysicsGeometryPlane {};

/**
 * @brief Capsule geometry data
 */
struct PhysicsGeometryCapsule {
  float radius = 1.0f;
  float halfHeight = 1.0f;
};

/**
 * @brief Box geometry data
 */
struct PhysicsGeometryBox {
  glm::vec3 halfExtents{1.0f};
};

using PhysicsGeometryParams =
    std::variant<liquid::PhysicsGeometryBox, liquid::PhysicsGeometrySphere,
                 liquid::PhysicsGeometryCapsule, liquid::PhysicsGeometryPlane>;

/**
 * @brief Describes geometry
 */
struct PhysicsGeometryDesc {
  PhysicsGeometryType type = PhysicsGeometryType::Box;
  PhysicsGeometryParams params = PhysicsGeometryBox{{0.0f, 0.0f, 0.0f}};
};

/**
 * @brief Describes dynamic rigid body
 */
struct PhysicsDynamicRigidBodyDesc {
  float mass = 1.0f;
  glm::vec3 inertia;
};

} // namespace liquid
