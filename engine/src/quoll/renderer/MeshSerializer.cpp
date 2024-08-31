#include "quoll/core/Base.h"
#include "Mesh.h"
#include "MeshSerializer.h"

namespace quoll {

void MeshSerializer::serialize(YAML::Node &node, EntityDatabase &entityDatabase,
                               Entity entity, AssetRegistry &assetRegistry) {
  if (entityDatabase.has<Mesh>(entity)) {
    auto handle = entityDatabase.get<Mesh>(entity).handle;
    if (assetRegistry.has(handle)) {
      auto uuid = assetRegistry.getMeta(handle).uuid;

      node["mesh"] = uuid;
    }
  }
}

void MeshSerializer::deserialize(const YAML::Node &node,
                                 EntityDatabase &entityDatabase, Entity entity,
                                 AssetRegistry &assetRegistry) {
  if (node["mesh"]) {
    auto uuid = node["mesh"].as<Uuid>(Uuid{});
    auto handle = assetRegistry.findHandleByUuid<MeshAsset>(uuid);

    if (handle) {
      entityDatabase.set<Mesh>(entity, {handle});
    }
  }
}

} // namespace quoll
