#include "quoll/core/Base.h"
#include "Mesh.h"
#include "MeshSerializer.h"
#include "SkinnedMesh.h"

namespace quoll {

void MeshSerializer::serialize(YAML::Node &node, EntityDatabase &entityDatabase,
                               Entity entity, AssetRegistry &assetRegistry) {
  if (entity.has<Mesh>()) {
    auto handle = entity.get_ref<Mesh>()->handle;
    if (assetRegistry.getMeshes().hasAsset(handle)) {
      auto uuid = assetRegistry.getMeshes().getAsset(handle).uuid;

      node["mesh"] = uuid;
    }
  } else if (entity.has<SkinnedMesh>()) {
    auto handle = entity.get_ref<SkinnedMesh>()->handle;
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
        entity.set<Mesh>({handle});
      } else if (type == AssetType::SkinnedMesh) {
        entity.set<SkinnedMesh>({handle});
      }
    }
  }
}

} // namespace quoll
