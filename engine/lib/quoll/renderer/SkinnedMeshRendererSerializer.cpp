#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "SkinnedMeshRenderer.h"
#include "SkinnedMeshRendererSerializer.h"

namespace quoll {

void SkinnedMeshRendererSerializer::serialize(YAML::Node &node,
                                              EntityDatabase &entityDatabase,
                                              Entity entity) {
  if (entityDatabase.has<SkinnedMeshRenderer>(entity)) {
    const auto &renderer = entityDatabase.get<SkinnedMeshRenderer>(entity);

    node["skinnedMeshRenderer"]["materials"] =
        YAML::Node(YAML::NodeType::Sequence);

    for (auto material : renderer.materials) {
      if (material) {
        node["skinnedMeshRenderer"]["materials"].push_back(
            material.meta().uuid);
      }
    }
  }
}

void SkinnedMeshRendererSerializer::deserialize(const YAML::Node &node,
                                                EntityDatabase &entityDatabase,
                                                Entity entity,
                                                AssetCache &assetCache) {
  if (node["skinnedMeshRenderer"] && node["skinnedMeshRenderer"].IsMap()) {
    SkinnedMeshRenderer renderer{};
    auto materials = node["skinnedMeshRenderer"]["materials"];

    if (materials.IsSequence()) {
      for (auto material : materials) {
        auto uuid = material.as<Uuid>(Uuid{});
        auto asset = assetCache.request<MaterialAsset>(uuid);
        if (!asset) {
          continue;
        }

        renderer.materials.push_back(asset);
      }
    }

    entityDatabase.set(entity, renderer);
  }
}

} // namespace quoll
