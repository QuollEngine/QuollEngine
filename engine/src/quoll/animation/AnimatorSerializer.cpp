#include "quoll/core/Base.h"
#include "AnimatorSerializer.h"

namespace quoll {

void AnimatorSerializer::serialize(YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity,
                                   AssetRegistry &assetRegistry) {
  if (entity.has<Animator>()) {
    auto handle = entity.get_ref<Animator>()->asset;

    if (assetRegistry.getAnimators().hasAsset(handle)) {
      auto uuid = assetRegistry.getAnimators().getAsset(handle).uuid;

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
    auto handle = assetRegistry.getAnimators().findHandleByUuid(assetUuid);

    if (handle != AnimatorAssetHandle::Null) {
      const auto &asset = assetRegistry.getAnimators().getAsset(handle);
      Animator animator;
      animator.asset = handle;
      entity.set(animator);
    }
  }
}

} // namespace quoll
