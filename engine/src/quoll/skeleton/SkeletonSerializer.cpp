#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "Skeleton.h"
#include "SkeletonSerializer.h"

namespace quoll {

void SkeletonSerializer::serialize(YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity) {
  if (entityDatabase.has<SkeletonAssetRef>(entity)) {
    const auto &asset = entityDatabase.get<SkeletonAssetRef>(entity).asset;
    if (asset.valid()) {
      node["skeleton"] = asset.meta().uuid;
    }
  }
}

void SkeletonSerializer::deserialize(const YAML::Node &node,
                                     EntityDatabase &entityDatabase,
                                     Entity entity, AssetCache &assetCache) {
  if (node["skeleton"]) {
    auto uuid = node["skeleton"].as<Uuid>(Uuid{});

    auto res = assetCache.request<SkeletonAsset>(uuid);
    if (res) {
      entityDatabase.set(entity, SkeletonAssetRef{res.data()});
    }
  }
}

} // namespace quoll
