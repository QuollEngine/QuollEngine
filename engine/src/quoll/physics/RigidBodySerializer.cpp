#include "quoll/core/Base.h"

#include "RigidBody.h"
#include "RigidBodySerializer.h"

namespace quoll {

void RigidBodySerializer::serialize(YAML::Node &node,
                                    EntityDatabase &entityDatabase,
                                    Entity entity) {
  if (entityDatabase.has<RigidBody>(entity)) {
    const auto &rigidBody = entityDatabase.get<RigidBody>(entity);

    if (rigidBody.type == RigidBodyType::Dynamic) {
      node["rigidBody"]["type"] = "dynamic";
      node["rigidBody"]["applyGravity"] = rigidBody.dynamicDesc.applyGravity;
      node["rigidBody"]["inertia"] = rigidBody.dynamicDesc.inertia;
      node["rigidBody"]["mass"] = rigidBody.dynamicDesc.mass;
    } else if (rigidBody.type == RigidBodyType::Kinematic) {
      node["rigidBody"]["type"] = "kinematic";
    }
  }
}

void RigidBodySerializer::deserialize(const YAML::Node &node,
                                      EntityDatabase &entityDatabase,
                                      Entity entity) {
  if (node["rigidBody"] && node["rigidBody"].IsMap()) {
    RigidBody rigidBody{};

    auto type = node["rigidBody"]["type"].as<quoll::String>("dynamic");
    if (type == "kinematic") {
      rigidBody.type = RigidBodyType::Kinematic;
    } else {
      rigidBody.type = RigidBodyType::Dynamic;
      rigidBody.dynamicDesc.mass =
          node["rigidBody"]["mass"].as<f32>(rigidBody.dynamicDesc.mass);
      rigidBody.dynamicDesc.inertia =
          node["rigidBody"]["inertia"].as<glm::vec3>(
              rigidBody.dynamicDesc.inertia);
      rigidBody.dynamicDesc.applyGravity =
          node["rigidBody"]["applyGravity"].as<bool>(
              rigidBody.dynamicDesc.applyGravity);
    }

    entityDatabase.set(entity, rigidBody);
  }
}

} // namespace quoll
