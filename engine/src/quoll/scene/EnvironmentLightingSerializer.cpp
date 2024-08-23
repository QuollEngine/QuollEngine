#include "quoll/core/Base.h"
#include "EnvironmentLighting.h"
#include "EnvironmentLightingSerializer.h"

namespace quoll {

void EnvironmentLightingSerializer::serialize(YAML::Node &node,
                                              EntityDatabase &entityDatabase,
                                              Entity entity) {
  if (entity.has<EnvironmentLightingSkyboxSource>()) {
    node["environmentLighting"]["source"] = "skybox";
  }
}

void EnvironmentLightingSerializer::deserialize(const YAML::Node &node,
                                                EntityDatabase &entityDatabase,
                                                Entity entity) {
  if (node["environmentLighting"] && node["environmentLighting"].IsMap()) {
    auto source = node["environmentLighting"]["source"].as<String>("");
    if (source == "skybox") {
      entity.add<EnvironmentLightingSkyboxSource>();
    }
  }
}

} // namespace quoll
