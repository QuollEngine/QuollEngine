#include "quoll/core/Base.h"
#include "quoll/core/Id.h"
#include "quoll/asset/AssetCache.h"
#include "Sprite.h"
#include "SpriteSerializer.h"

namespace quoll {

void SpriteSerializer::serialize(YAML::Node &node,
                                 EntityDatabase &entityDatabase,
                                 Entity entity) {
  if (entityDatabase.has<Sprite>(entity)) {
    const auto &texture = entityDatabase.get<Sprite>(entity).texture;
    if (texture) {
      node["sprite"] = texture.meta().uuid;
    }
  }
}

void SpriteSerializer::deserialize(const YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity, AssetCache &assetCache) {
  if (node["sprite"]) {
    auto uuid = node["sprite"].as<Uuid>(Uuid{});

    auto texture = assetCache.request<TextureAsset>(uuid);
    if (texture) {
      entityDatabase.set<Sprite>(entity, {texture});
    }
  }
}

} // namespace quoll
