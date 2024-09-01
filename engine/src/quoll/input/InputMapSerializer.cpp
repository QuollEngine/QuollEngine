#include "quoll/core/Base.h"
#include "InputMap.h"
#include "InputMapSerializer.h"

namespace quoll {

void InputMapSerializer::serialize(YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity) {
  if (entityDatabase.has<InputMapAssetRef>(entity)) {
    const auto &component = entityDatabase.get<InputMapAssetRef>(entity);

    if (component.asset) {
      node["inputMap"]["asset"] = component.asset.meta().uuid;
      node["inputMap"]["defaultScheme"] = component.defaultScheme;
    }
  }
}

void InputMapSerializer::deserialize(const YAML::Node &node,
                                     EntityDatabase &entityDatabase,
                                     Entity entity, AssetCache &assetCache) {

  if (node["inputMap"] && node["inputMap"].IsMap()) {
    auto uuid = node["inputMap"]["asset"].as<Uuid>(Uuid{});
    auto defaultScheme = node["inputMap"]["defaultScheme"].as<usize>(0);

    auto asset = assetCache.request<InputMapAsset>(uuid);
    if (asset) {
      entityDatabase.set<InputMapAssetRef>(entity, {asset, defaultScheme});
    }
  }
}

} // namespace quoll
