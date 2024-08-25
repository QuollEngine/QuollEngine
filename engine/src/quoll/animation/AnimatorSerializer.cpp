#include "quoll/core/Base.h"
#include "AnimatorSerializer.h"

namespace quoll {

void AnimatorSerializer::serialize(YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity,
                                   AssetRegistry &assetRegistry) {
  if (entityDatabase.has<Animator>(entity)) {
    auto handle = entityDatabase.get<Animator>(entity).asset;

    if (assetRegistry.has(handle)) {
      auto uuid = assetRegistry.get(handle).uuid;

      node["animator"]["asset"] = uuid;
    }
  }
}

void AnimatorSerializer::deserialize(const YAML::Node &node,
                                     EntityDatabase &entityDatabase,
                                     Entity entity,
                                     AssetRegistry &assetRegistry) {
  if (node["animator"] && node["animator"].IsMap() &&
      node["animator"]["asset"]) {
    auto assetUuid = node["animator"]["asset"].as<Uuid>(Uuid{});
    auto handle = assetRegistry.findHandleByUuid<AnimatorAsset>(assetUuid);

    if (handle) {
      const auto &asset = assetRegistry.get(handle);
      Animator animator;
      animator.asset = handle;
      entityDatabase.set(entity, animator);
    }
  }
}

} // namespace quoll
