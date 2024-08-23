#include "quoll/core/Base.h"
#include "InputMap.h"
#include "InputMapSerializer.h"

namespace quoll {

void InputMapSerializer::serialize(YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity,
                                   AssetRegistry &assetRegistry) {
  if (entity.has<InputMapAssetRef>()) {
    auto component = entity.get_ref<InputMapAssetRef>();

    if (assetRegistry.getInputMaps().hasAsset(component->handle)) {
      node["inputMap"]["asset"] =
          assetRegistry.getInputMaps().getAsset(component->handle).uuid;
      node["inputMap"]["defaultScheme"] = component->defaultScheme;
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
    auto handle = assetRegistry.getInputMaps().findHandleByUuid(uuid);

    if (handle != InputMapAssetHandle::Null) {
      auto type = assetRegistry.getInputMaps().getAsset(handle).type;

      entity.set<InputMapAssetRef>({handle, defaultScheme});
    }
  }
}

} // namespace quoll
