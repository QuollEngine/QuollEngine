#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "MeshRenderer.h"
#include "MeshRendererSerializer.h"

namespace quoll {

void MeshRendererSerializer::serialize(YAML::Node &node,
                                       EntityDatabase &entityDatabase,
                                       Entity entity) {
  if (entityDatabase.has<MeshRenderer>(entity)) {
    const auto &renderer = entityDatabase.get<MeshRenderer>(entity);

    node["meshRenderer"]["materials"] = YAML::Node(YAML::NodeType::Sequence);

    for (auto material : renderer.materials) {
      if (material) {
        node["meshRenderer"]["materials"].push_back(material.meta().uuid);
      }
    }
  }
}

void MeshRendererSerializer::deserialize(const YAML::Node &node,
                                         EntityDatabase &entityDatabase,
                                         Entity entity,
                                         AssetCache &assetCache) {
  if (node["meshRenderer"] && node["meshRenderer"].IsMap()) {
    MeshRenderer renderer{};
    auto materials = node["meshRenderer"]["materials"];

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
