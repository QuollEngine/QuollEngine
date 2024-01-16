#include "quoll/core/Base.h"

#include "MeshSerializer.h"
#include "Mesh.h"
#include "SkinnedMesh.h"

namespace quoll {

void MeshSerializer::serialize(YAML::Node &node, EntityDatabase &entityDatabase,
                               Entity entity, AssetRegistry &assetRegistry) {
  if (entityDatabase.has<Mesh>(entity)) {
    auto handle = entityDatabase.get<Mesh>(entity).handle;
    if (assetRegistry.getMeshes().hasAsset(handle)) {
      auto uuid = assetRegistry.getMeshes().getAsset(handle).uuid;

      node["mesh"] = uuid;
    }
  } else if (entityDatabase.has<SkinnedMesh>(entity)) {
    auto handle = entityDatabase.get<SkinnedMesh>(entity).handle;
    if (assetRegistry.getMeshes().hasAsset(handle)) {
      auto uuid = assetRegistry.getMeshes().getAsset(handle).uuid;

      node["mesh"] = uuid;
    }
  }
}

void MeshSerializer::deserialize(const YAML::Node &node,
                                 EntityDatabase &entityDatabase, Entity entity,
                                 AssetRegistry &assetRegistry) {
  if (node["mesh"]) {
    auto uuid = node["mesh"].as<Uuid>(Uuid{});
    auto handle = assetRegistry.getMeshes().findHandleByUuid(uuid);

    if (handle != MeshAssetHandle::Null) {
      auto type = assetRegistry.getMeshes().getAsset(handle).type;

      if (type == AssetType::Mesh) {
        entityDatabase.set<Mesh>(entity, {handle});
      } else if (type == AssetType::SkinnedMesh) {
        entityDatabase.set<SkinnedMesh>(entity, {handle});
      }
    }
  }
}

} // namespace quoll
