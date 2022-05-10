#pragma once

namespace liquid {

/**
 * @brief Describes material attached to a shape
 */
struct PhysicsMaterialDesc {
  /**
   * Static friction
   */
  float staticFriction = 0.0f;

  /**
   * Dynamic friction
   */
  float dynamicFriction = 0.0f;

  /**
   * Restitution
   */
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
  /**
   * Sphere radius
   */
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
  /**
   * Capsule radius
   */
  float radius = 1.0f;

  /**
   * Capsule half height
   */
  float halfHeight = 1.0f;
};

/**
 * @brief Box geometry data
 */
struct PhysicsGeometryBox {
  /**
   * Box extents halved
   */
  glm::vec3 halfExtents{1.0f};
};

using PhysicsGeometryParams =
    std::variant<liquid::PhysicsGeometryBox, liquid::PhysicsGeometrySphere,
                 liquid::PhysicsGeometryCapsule, liquid::PhysicsGeometryPlane>;

/**
 * @brief Describes geometry
 */
struct PhysicsGeometryDesc {
  /**
   * Geometry type
   */
  PhysicsGeometryType type = PhysicsGeometryType::Box;

  /**
   * Geometry parameters
   */
  PhysicsGeometryParams params = PhysicsGeometryBox{{0.0f, 0.0f, 0.0f}};
};

/**
 * @brief Describes dynamic rigid body
 */
struct PhysicsDynamicRigidBodyDesc {
  /**
   * Mass
   */
  float mass = 1.0f;

  /**
   * Inertia
   */
  glm::vec3 inertia;

  /**
   * Apply gravity
   */
  bool applyGravity = true;
};

} // namespace liquid
