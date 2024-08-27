#include "quoll/core/Base.h"
#include "InputMap.h"
#include "InputMapSerializer.h"

namespace quoll {

void InputMapSerializer::serialize(YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity,
                                   AssetRegistry &assetRegistry) {
  if (entityDatabase.has<InputMapAssetRef>(entity)) {
    const auto &component = entityDatabase.get<InputMapAssetRef>(entity);

    if (assetRegistry.has(component.handle)) {
      node["inputMap"]["asset"] = assetRegistry.getMeta(component.handle).uuid;
      node["inputMap"]["defaultScheme"] = component.defaultScheme;
    }
  }
}

void InputMapSerializer::deserialize(const YAML::Node &node,
                                     EntityDatabase &entityDatabase,
                                     Entity entity,
                                     AssetRegistry &assetRegistry) {

  if (node["inputMap"] && node["inputMap"].IsMap()) {
    auto uuid = node["inputMap"]["asset"].as<Uuid>(Uuid{});
    auto defaultScheme = node["inputMap"]["defaultScheme"].as<usize>(0);
    auto handle = assetRegistry.findHandleByUuid<InputMapAsset>(uuid);

    if (handle) {
      auto type = assetRegistry.getMeta(handle).type;

      entityDatabase.set<InputMapAssetRef>(entity, {handle, defaultScheme});
    }
  }
}

} // namespace quoll
