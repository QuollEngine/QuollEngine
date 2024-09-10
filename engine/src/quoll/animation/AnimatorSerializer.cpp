#include "quoll/core/Base.h"
#include "AnimatorSerializer.h"

namespace quoll {

void AnimatorSerializer::serialize(YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity) {
  if (entityDatabase.has<AnimatorAssetRef>(entity)) {
    const auto &asset = entityDatabase.get<AnimatorAssetRef>(entity).asset;

    if (asset.valid()) {
      node["animator"]["asset"] = asset.meta().uuid;
    }
  }
}

void AnimatorSerializer::deserialize(const YAML::Node &node,
                                     EntityDatabase &entityDatabase,
                                     Entity entity, AssetCache &assetCache) {
  if (node["animator"] && node["animator"].IsMap() &&
      node["animator"]["asset"]) {
    auto uuid = node["animator"]["asset"].as<Uuid>(Uuid{});
    auto ref = assetCache.request<AnimatorAsset>(uuid);

    if (ref) {
      entityDatabase.set(entity, AnimatorAssetRef{ref});
    }
  }
}

} // namespace quoll
