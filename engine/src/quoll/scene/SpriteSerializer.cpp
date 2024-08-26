#include "quoll/core/Base.h"
#include "quoll/core/Id.h"
#include "Sprite.h"
#include "SpriteSerializer.h"

namespace quoll {

void SpriteSerializer::serialize(YAML::Node &node,
                                 EntityDatabase &entityDatabase, Entity entity,
                                 AssetRegistry &assetRegistry) {
  if (entityDatabase.has<Sprite>(entity)) {
    auto handle = entityDatabase.get<Sprite>(entity).handle;
    if (assetRegistry.has(handle)) {
      auto uuid = assetRegistry.getMeta(handle).uuid;

      node["sprite"] = uuid;
    }
  }
}

void SpriteSerializer::deserialize(const YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity,
                                   AssetRegistry &assetRegistry) {
  if (node["sprite"]) {
    auto uuid = node["sprite"].as<Uuid>(Uuid{});

    auto handle = assetRegistry.findHandleByUuid<TextureAsset>(uuid);

    if (handle) {
      entityDatabase.set<Sprite>(entity, {handle});
    }
  }
}

} // namespace quoll
