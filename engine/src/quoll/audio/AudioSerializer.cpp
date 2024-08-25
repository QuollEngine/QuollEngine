#include "quoll/core/Base.h"
#include "AudioSerializer.h"
#include "AudioSource.h"

namespace quoll {

void AudioSerializer::serialize(YAML::Node &node,
                                EntityDatabase &entityDatabase, Entity entity,
                                AssetRegistry &assetRegistry) {
  if (entityDatabase.has<AudioSource>(entity)) {
    auto handle = entityDatabase.get<AudioSource>(entity).source;
    if (assetRegistry.has(handle)) {
      auto uuid = assetRegistry.get(handle).uuid;

      node["audio"]["source"] = uuid;
    }
  }
}

void AudioSerializer::deserialize(const YAML::Node &node,
                                  EntityDatabase &entityDatabase, Entity entity,
                                  AssetRegistry &assetRegistry) {
  if (node["audio"] && node["audio"].IsMap()) {
    auto uuid = node["audio"]["source"].as<Uuid>(Uuid{});

    auto handle = assetRegistry.findHandleByUuid<AudioAsset>(uuid);

    if (handle) {
      entityDatabase.set<AudioSource>(entity, {handle});
    }
  }
}

} // namespace quoll
