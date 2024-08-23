#include "quoll/core/Base.h"
#include "AudioSerializer.h"
#include "AudioSource.h"

namespace quoll {

void AudioSerializer::serialize(YAML::Node &node,
                                EntityDatabase &entityDatabase, Entity entity,
                                AssetRegistry &assetRegistry) {
  if (entity.has<AudioSource>()) {
    auto handle = entity.get_ref<AudioSource>()->source;
    if (assetRegistry.getAudios().hasAsset(handle)) {
      auto uuid = assetRegistry.getAudios().getAsset(handle).uuid;

      node["audio"]["source"] = uuid;
    }
  }
}

void AudioSerializer::deserialize(const YAML::Node &node,
                                  EntityDatabase &entityDatabase, Entity entity,
                                  AssetRegistry &assetRegistry) {
  if (node["audio"] && node["audio"].IsMap()) {
    auto uuid = node["audio"]["source"].as<Uuid>(Uuid{});

    auto handle = assetRegistry.getAudios().findHandleByUuid(uuid);

    if (handle != AudioAssetHandle::Null) {
      entity.set<AudioSource>({handle});
    }
  }
}

} // namespace quoll
