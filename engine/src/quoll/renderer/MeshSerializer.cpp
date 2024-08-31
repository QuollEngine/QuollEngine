#include "quoll/core/Base.h"
#include "Mesh.h"
#include "MeshSerializer.h"

namespace quoll {

void MeshSerializer::serialize(YAML::Node &node, EntityDatabase &entityDatabase,
                               Entity entity) {
  if (entityDatabase.has<Mesh>(entity)) {
    const auto &mesh = entityDatabase.get<Mesh>(entity);
    if (mesh.handle) {
      node["mesh"] = mesh.handle.meta().uuid;
    }
  }
}

void MeshSerializer::deserialize(const YAML::Node &node,
                                 EntityDatabase &entityDatabase, Entity entity,
                                 AssetCache &assetCache) {
  if (node["mesh"]) {
    auto uuid = node["mesh"].as<Uuid>(Uuid{});
    auto asset = assetCache.request<MeshAsset>(uuid);

    if (asset) {
      entityDatabase.set<Mesh>(entity, {asset});
    }
  }
}

} // namespace quoll
