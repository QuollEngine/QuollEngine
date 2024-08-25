#include "quoll/core/Base.h"
#include "SkinnedMeshRendererSerializer.h"

namespace quoll {

void SkinnedMeshRendererSerializer::serialize(YAML::Node &node,
                                              EntityDatabase &entityDatabase,
                                              Entity entity,
                                              AssetRegistry &assetRegistry) {
  if (entityDatabase.has<SkinnedMeshRenderer>(entity)) {
    const auto &renderer = entityDatabase.get<SkinnedMeshRenderer>(entity);

    node["skinnedMeshRenderer"]["materials"] =
        YAML::Node(YAML::NodeType::Sequence);

    for (auto material : renderer.materials) {
      if (assetRegistry.has(material)) {
        auto uuid = assetRegistry.get(material).uuid;
        node["skinnedMeshRenderer"]["materials"].push_back(uuid);
      }
    }
  }
}

void SkinnedMeshRendererSerializer::deserialize(const YAML::Node &node,
                                                EntityDatabase &entityDatabase,
                                                Entity entity,
                                                AssetRegistry &assetRegistry) {
  if (node["skinnedMeshRenderer"] && node["skinnedMeshRenderer"].IsMap()) {
    SkinnedMeshRenderer renderer{};
    auto materials = node["skinnedMeshRenderer"]["materials"];

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
