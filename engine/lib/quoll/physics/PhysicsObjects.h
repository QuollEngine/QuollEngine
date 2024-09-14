#pragma once

namespace quoll {

struct PhysicsMaterialDesc {
  f32 staticFriction = 0.0f;

  f32 dynamicFriction = 0.0f;

  f32 restitution = 1.0f;
};

enum class PhysicsGeometryType { Box, Sphere, Capsule, Plane };

inline String getPhysicsGeometryTypeString(PhysicsGeometryType type) {
  switch (type) {
  case PhysicsGeometryType::Box:
    return "box";
  case PhysicsGeometryType::Sphere:
    return "sphere";
  case PhysicsGeometryType::Capsule:
    return "capsule";
  case PhysicsGeometryType::Plane:
    return "plane";
  default:
    return "unknown";
  }
}

struct PhysicsGeometrySphere {
  f32 radius = 1.0f;
};

struct PhysicsGeometryPlane {};

struct PhysicsGeometryCapsule {
  f32 radius = 1.0f;

  f32 halfHeight = 0.5f;
};

struct PhysicsGeometryBox {
  glm::vec3 halfExtents{0.5f};
};

using PhysicsGeometryParams =
    std::variant<quoll::PhysicsGeometryBox, quoll::PhysicsGeometrySphere,
                 quoll::PhysicsGeometryCapsule, quoll::PhysicsGeometryPlane>;

struct PhysicsGeometryDesc {
  PhysicsGeometryType type = PhysicsGeometryType::Box;

  glm::vec3 center{0.0f};

  PhysicsGeometryParams params = PhysicsGeometryBox{};
};

struct PhysicsDynamicRigidBodyDesc {
  f32 mass = 1.0f;

  glm::vec3 inertia;

  bool applyGravity = true;
};

} // namespace quoll
