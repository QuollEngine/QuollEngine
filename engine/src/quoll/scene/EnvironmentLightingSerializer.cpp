#include "quoll/core/Base.h"

#include "EnvironmentLightingSerializer.h"
#include "EnvironmentLighting.h"

namespace quoll {

void EnvironmentLightingSerializer::serialize(YAML::Node &node,
                                              EntityDatabase &entityDatabase,
                                              Entity entity) {
  if (entityDatabase.has<EnvironmentLightingSkyboxSource>(entity)) {
    node["environmentLighting"]["source"] = "skybox";
  }
}

void EnvironmentLightingSerializer::deserialize(const YAML::Node &node,
                                                EntityDatabase &entityDatabase,
                                                Entity entity) {
  if (node["environmentLighting"] && node["environmentLighting"].IsMap()) {
    auto source = node["environmentLighting"]["source"].as<String>("");
    if (source == "skybox") {
      entityDatabase.set<EnvironmentLightingSkyboxSource>(entity, {});
    }
  }
}

} // namespace quoll
