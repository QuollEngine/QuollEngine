#include "quoll/core/Base.h"
#include "AudioSerializer.h"
#include "AudioSource.h"

namespace quoll {

void AudioSerializer::serialize(YAML::Node &node,
                                EntityDatabase &entityDatabase, Entity entity) {
  if (entityDatabase.has<AudioSource>(entity)) {
    const auto &asset = entityDatabase.get<AudioSource>(entity).asset;
    if (asset) {
      node["audio"]["source"] = asset.meta().uuid;
    }
  }
}

void AudioSerializer::deserialize(const YAML::Node &node,
                                  EntityDatabase &entityDatabase, Entity entity,
                                  AssetCache &assetCache) {
  if (node["audio"] && node["audio"].IsMap()) {
    auto uuid = node["audio"]["source"].as<Uuid>(Uuid{});
    auto asset = assetCache.request<AudioAsset>(uuid);

    if (asset) {
      entityDatabase.set<AudioSource>(entity, {asset});
    }
  }
}

} // namespace quoll
