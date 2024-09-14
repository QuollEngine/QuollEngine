#include "quoll/core/Base.h"
#include "Collidable.h"
#include "CollidableSerializer.h"

namespace quoll {

void CollidableSerializer::serialize(YAML::Node &node,
                                     EntityDatabase &entityDatabase,
                                     Entity entity) {
  if (entityDatabase.has<Collidable>(entity)) {
    const auto &component = entityDatabase.get<Collidable>(entity);

    auto type = component.geometryDesc.type;

    node["collidable"]["shape"] = getPhysicsGeometryTypeString(type);
    node["collidable"]["center"] = component.geometryDesc.center;
    node["collidable"]["useInSimulation"] = component.useInSimulation;
    node["collidable"]["useInQueries"] = component.useInQueries;

    if (type == PhysicsGeometryType::Box) {
      node["collidable"]["halfExtents"] =
          std::get<PhysicsGeometryBox>(component.geometryDesc.params)
              .halfExtents;
    } else if (type == PhysicsGeometryType::Sphere) {
      node["collidable"]["radius"] =
          std::get<PhysicsGeometrySphere>(component.geometryDesc.params).radius;
    } else if (type == PhysicsGeometryType::Capsule) {
      const auto &capsule =
          std::get<PhysicsGeometryCapsule>(component.geometryDesc.params);
      node["collidable"]["radius"] = capsule.radius;
      node["collidable"]["halfHeight"] = capsule.halfHeight;
    }

    node["collidable"]["dynamicFriction"] =
        component.materialDesc.dynamicFriction;
    node["collidable"]["restitution"] = component.materialDesc.restitution;
    node["collidable"]["staticFriction"] =
        component.materialDesc.staticFriction;
  }
}

void CollidableSerializer::deserialize(const YAML::Node &node,
                                       EntityDatabase &entityDatabase,
                                       Entity entity) {
  static const std::unordered_map<String, PhysicsGeometryType> ValidShapes{
      {"box", PhysicsGeometryType::Box},
      {"sphere", PhysicsGeometryType::Sphere},
      {"capsule", PhysicsGeometryType::Capsule},
      {"plane", PhysicsGeometryType::Plane}};

  if (node["collidable"] && node["collidable"].IsMap() &&
      ValidShapes.find(node["collidable"]["shape"].as<String>("unknown")) !=
          ValidShapes.end()) {
    Collidable collidable{};
    auto shape = ValidShapes.at(node["collidable"]["shape"].as<String>());
    collidable.geometryDesc.type = shape;
    collidable.geometryDesc.center = node["collidable"]["center"].as<glm::vec3>(
        collidable.geometryDesc.center);
    collidable.useInSimulation = node["collidable"]["useInSimulation"].as<bool>(
        collidable.useInSimulation);
    collidable.useInQueries =
        node["collidable"]["useInQueries"].as<bool>(collidable.useInQueries);

    if (shape == PhysicsGeometryType::Box) {
      quoll::PhysicsGeometryBox box{};
      box.halfExtents =
          node["collidable"]["halfExtents"].as<glm::vec3>(box.halfExtents);

      collidable.geometryDesc.params = box;
    } else if (shape == PhysicsGeometryType::Sphere) {
      quoll::PhysicsGeometrySphere sphere{};
      sphere.radius = node["collidable"]["radius"].as<f32>(sphere.radius);

      collidable.geometryDesc.params = sphere;
    } else if (shape == PhysicsGeometryType::Capsule) {
      quoll::PhysicsGeometryCapsule capsule{};
      capsule.radius = node["collidable"]["radius"].as<f32>(capsule.radius);
      capsule.halfHeight =
          node["collidable"]["halfHeight"].as<f32>(capsule.halfHeight);

      collidable.geometryDesc.params = capsule;
    } else if (shape == PhysicsGeometryType::Plane) {
      collidable.geometryDesc.params = PhysicsGeometryPlane{};
    }

    collidable.materialDesc.dynamicFriction =
        node["collidable"]["dynamicFriction"].as<f32>(
            collidable.materialDesc.dynamicFriction);
    collidable.materialDesc.restitution =
        node["collidable"]["restitution"].as<f32>(
            collidable.materialDesc.restitution);
    collidable.materialDesc.staticFriction =
        node["collidable"]["staticFriction"].as<f32>(
            collidable.materialDesc.staticFriction);

    entityDatabase.set(entity, collidable);
  }
}

} // namespace quoll
