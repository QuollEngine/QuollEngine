#include "quoll/core/Base.h"
#include "MeshRendererSerializer.h"

namespace quoll {

void MeshRendererSerializer::serialize(YAML::Node &node,
                                       EntityDatabase &entityDatabase,
                                       Entity entity,
                                       AssetRegistry &assetRegistry) {
  if (entityDatabase.has<MeshRenderer>(entity)) {
    const auto &renderer = entityDatabase.get<MeshRenderer>(entity);

    node["meshRenderer"]["materials"] = YAML::Node(YAML::NodeType::Sequence);

    for (auto material : renderer.materials) {
      if (assetRegistry.has(material)) {
        auto uuid = assetRegistry.get(material).uuid;
        node["meshRenderer"]["materials"].push_back(uuid);
      }
    }
  }
}

void MeshRendererSerializer::deserialize(const YAML::Node &node,
                                         EntityDatabase &entityDatabase,
                                         Entity entity,
                                         AssetRegistry &assetRegistry) {
  if (node["meshRenderer"] && node["meshRenderer"].IsMap()) {
    MeshRenderer renderer{};
    auto materials = node["meshRenderer"]["materials"];

    if (materials.IsSequence()) {
      for (auto material : materials) {
        auto uuid = material.as<Uuid>(Uuid{});

        auto handle = assetRegistry.findHandleByUuid<MaterialAsset>(uuid);
        if (!handle) {
          continue;
        }

        renderer.materials.push_back(handle);
      }
    }

    entityDatabase.set(entity, renderer);
  }
}

} // namespace quoll
