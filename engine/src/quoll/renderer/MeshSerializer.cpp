#include "quoll/core/Base.h"
#include "Mesh.h"
#include "MeshSerializer.h"
#include "SkinnedMesh.h"

namespace quoll {

void MeshSerializer::serialize(YAML::Node &node, EntityDatabase &entityDatabase,
                               Entity entity, AssetRegistry &assetRegistry) {
  if (entityDatabase.has<Mesh>(entity)) {
    auto handle = entityDatabase.get<Mesh>(entity).handle;
    if (assetRegistry.has(handle)) {
      auto uuid = assetRegistry.get(handle).uuid;

      node["mesh"] = uuid;
    }
  } else if (entityDatabase.has<SkinnedMesh>(entity)) {
    auto handle = entityDatabase.get<SkinnedMesh>(entity).handle;
    if (assetRegistry.has(handle)) {
      auto uuid = assetRegistry.get(handle).uuid;

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
      auto type = assetRegistry.get(handle).type;

      if (type == AssetType::Mesh) {
        entityDatabase.set<Mesh>(entity, {handle});
      } else if (type == AssetType::SkinnedMesh) {
        entityDatabase.set<SkinnedMesh>(entity, {handle});
      }
    }
  }
}

} // namespace quoll
