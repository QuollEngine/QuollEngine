#include "quoll/core/Base.h"
#include "AnimatorSerializer.h"

namespace quoll {

void AnimatorSerializer::serialize(YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity) {
  if (entityDatabase.has<Animator>(entity)) {
    const auto &asset = entityDatabase.get<Animator>(entity).asset;

    if (asset) {
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
    auto asset = assetCache.request<AnimatorAsset>(uuid);

    if (asset) {
      Animator animator;
      animator.asset = asset;
      entityDatabase.set(entity, animator);
    }
  }
}

} // namespace quoll
