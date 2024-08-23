#include "quoll/core/Base.h"
#include "SkinnedMeshRendererSerializer.h"

namespace quoll {

void SkinnedMeshRendererSerializer::serialize(YAML::Node &node,
                                              EntityDatabase &entityDatabase,
                                              Entity entity,
                                              AssetRegistry &assetRegistry) {
  if (entity.has<SkinnedMeshRenderer>()) {
    auto renderer = entity.get_ref<SkinnedMeshRenderer>();

    node["skinnedMeshRenderer"]["materials"] =
        YAML::Node(YAML::NodeType::Sequence);

    for (auto material : renderer->materials) {
      if (assetRegistry.getMaterials().hasAsset(material)) {
        auto uuid = assetRegistry.getMaterials().getAsset(material).uuid;
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
        auto handle = assetRegistry.getMaterials().findHandleByUuid(uuid);
        if (handle == MaterialAssetHandle::Null) {
          continue;
        }

        renderer.materials.push_back(handle);
      }
    }

    entity.set(renderer);
  }
}

} // namespace quoll
