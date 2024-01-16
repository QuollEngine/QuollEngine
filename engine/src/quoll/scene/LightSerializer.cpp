#include "quoll/core/Base.h"

#include "LightSerializer.h"
#include "CascadedShadowMap.h"

namespace quoll {

void LightSerializer::serialize(YAML::Node &node,
                                EntityDatabase &entityDatabase, Entity entity) {
  if (entityDatabase.has<DirectionalLight>(entity)) {
    const auto &light = entityDatabase.get<DirectionalLight>(entity);

    node["light"]["type"] = 0;
    node["light"]["color"] = light.color;
    node["light"]["intensity"] = light.intensity;

    if (entityDatabase.has<CascadedShadowMap>(entity)) {
      const auto &shadow = entityDatabase.get<CascadedShadowMap>(entity);
      node["light"]["shadow"]["softShadows"] = shadow.softShadows;
      node["light"]["shadow"]["splitLambda"] = shadow.splitLambda;
      node["light"]["shadow"]["numCascades"] = shadow.numCascades;
    }
  } else if (entityDatabase.has<PointLight>(entity)) {
    const auto &light = entityDatabase.get<PointLight>(entity);

    node["light"]["type"] = 1;
    node["light"]["color"] = light.color;
    node["light"]["intensity"] = light.intensity;
    node["light"]["range"] = light.range;
  }
}

void LightSerializer::deserialize(const YAML::Node &node,
                                  EntityDatabase &entityDatabase,
                                  Entity entity) {
  if (node["light"] && node["light"].IsMap()) {
    auto light = node["light"];
    auto type = light["type"].as<u32>(std::numeric_limits<u32>::max());

    if (type == 0) {
      DirectionalLight component{};
      component.intensity = light["intensity"].as<f32>(component.intensity);
      component.color = light["color"].as<glm::vec4>(component.color);

      entityDatabase.set(entity, component);

      if (light["shadow"] && light["shadow"].IsMap()) {
        CascadedShadowMap shadowComponent{};
        shadowComponent.softShadows = light["shadow"]["softShadows"].as<bool>(
            shadowComponent.softShadows);
        shadowComponent.splitLambda =
            light["shadow"]["splitLambda"].as<f32>(shadowComponent.splitLambda);
        shadowComponent.numCascades =
            light["shadow"]["numCascades"].as<u32>(shadowComponent.numCascades);

        shadowComponent.numCascades = glm::clamp(
            shadowComponent.numCascades, 1u, shadowComponent.MaxCascades);

        shadowComponent.splitLambda =
            glm::clamp(shadowComponent.splitLambda, 0.0f, 1.0f);

        entityDatabase.set(entity, shadowComponent);
      }
    } else if (type == 1) {
      PointLight component{};
      component.intensity = light["intensity"].as<f32>(component.intensity);
      component.color = light["color"].as<glm::vec4>(component.color);
      component.range = light["range"].as<glm::float32>(component.range);

      entityDatabase.set(entity, component);
    }
  }
}

} // namespace quoll
