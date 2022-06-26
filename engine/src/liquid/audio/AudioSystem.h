#pragma once

#include "liquid/asset/AssetRegistry.h"
#include "liquid/entity/EntityDatabase.h"

#include "liquid/audio/DefaultAudioBackend.h"

namespace liquid {

/**
 * @brief Audio system
 *
 * @tparam Backend Audio backend
 */
template <class AudioBackend = DefaultAudioBackend> class AudioSystem {
public:
  /**
   * @brief Create audio system
   *
   * @param assetRegistry Asset regsitry
   */
  AudioSystem(AssetRegistry &assetRegistry) : mAssetRegistry(assetRegistry) {}

  /**
   * @brief Output all sounds to device
   *
   * @param entityDatabase Entity database
   */
  void output(EntityDatabase &entityDatabase) {
    entityDatabase.iterateEntities<AudioSourceComponent, AudioStartComponent>(
        [this, &entityDatabase](auto entity, const auto &source,
                                const auto &play) {
          void *data =
              mAssetRegistry.getAudios().getAsset(source.source).data.data;
          void *sound = mBackend.playSound(data);

          entityDatabase.setComponent<AudioStatusComponent>(entity, {sound});
        });

    entityDatabase.destroyComponents<liquid::AudioStartComponent>();

    std::vector<Entity> toBeDeleted;
    entityDatabase.iterateEntities<AudioStatusComponent>(
        [&toBeDeleted, this](auto entity, const auto &status) {
          if (!mBackend.isPlaying(status.instance)) {
            mBackend.destroySound(status.instance);
            toBeDeleted.push_back(entity);
          }
        });

    for (auto entity : toBeDeleted) {
      entityDatabase.deleteComponent<AudioStatusComponent>(entity);
    }
  }

  /**
   * @brief Cleanup all the audio instances
   *
   * @param entityDatabase Entity database
   */
  void cleanup(EntityDatabase &entityDatabase) {
    entityDatabase.iterateEntities<AudioStatusComponent>(
        [this](auto entity, const auto &status) {
          mBackend.destroySound(status.instance);
        });
  }

  /**
   * @brief Get audio backend
   *
   * @return Audio backend
   */
  inline AudioBackend &getBackend() { return mBackend; }

private:
  AudioBackend mBackend;
  AssetRegistry &mAssetRegistry;
};

} // namespace liquid
