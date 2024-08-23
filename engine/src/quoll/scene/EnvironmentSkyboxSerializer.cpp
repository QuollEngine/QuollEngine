#include "quoll/core/Base.h"
#include "EnvironmentSkybox.h"
#include "EnvironmentSkyboxSerializer.h"

namespace quoll {

void EnvironmentSkyboxSerializer::serialize(YAML::Node &node,
                                            EntityDatabase &entityDatabase,
                                            Entity entity,
                                            AssetRegistry &assetRegistry) {
  if (entity.has<EnvironmentSkybox>()) {
    auto component = entity.get_ref<EnvironmentSkybox>();
    if (component->type == EnvironmentSkyboxType::Color) {
      node["skybox"]["type"] = "color";
      node["skybox"]["color"] = component->color;
    } else if (component->type == EnvironmentSkyboxType::Texture) {
      if (assetRegistry.getEnvironments().hasAsset(component->texture)) {
        const auto &asset =
            assetRegistry.getEnvironments().getAsset(component->texture);
        node["skybox"]["type"] = "texture";
        node["skybox"]["texture"] = asset.uuid;
      }
    }
  }
}

void EnvironmentSkyboxSerializer::deserialize(const YAML::Node &node,
                                              EntityDatabase &entityDatabase,
                                              Entity entity,
                                              AssetRegistry &assetRegistry) {
  if (node["skybox"] && node["skybox"].IsMap()) {
    EnvironmentSkybox skybox{};
    auto type = node["skybox"]["type"].as<String>("");
    if (type == "color") {
      skybox.type = EnvironmentSkyboxType::Color;
      skybox.color = node["skybox"]["color"].as<glm::vec4>(
          glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
      entity.set(skybox);
    } else if (type == "texture") {
      auto uuid = node["skybox"]["texture"].as<Uuid>(Uuid{});
      auto handle = assetRegistry.getEnvironments().findHandleByUuid(uuid);
      if (handle != EnvironmentAssetHandle::Null) {
        skybox.type = EnvironmentSkyboxType::Texture;
        skybox.texture = handle;
        entity.set(skybox);
      }
    }
  }
}

} // namespace quoll
