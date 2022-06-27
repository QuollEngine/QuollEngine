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
    LIQUID_PROFILE_EVENT("AudioSystem::output");

    {
      LIQUID_PROFILE_EVENT("Cleanup audio starts for deleted components");

      std::set<Entity> toBeDeletedStarts;
      entityDatabase.iterateEntities<AudioStartComponent, DeleteComponent>(
          [this, &toBeDeletedStarts](auto entity, auto &status, const auto &) {
            toBeDeletedStarts.insert(entity);
          });

      for (auto entity : toBeDeletedStarts) {
        entityDatabase.deleteComponent<AudioStartComponent>(entity);
      }
    }

    {
      LIQUID_PROFILE_EVENT("Cleanup audio statuses for deleted components");

      std::set<Entity> toBeDeletedStatuses;
      entityDatabase.iterateEntities<AudioStatusComponent, DeleteComponent>(
          [this, &toBeDeletedStatuses](auto entity, auto &status,
                                       const auto &) {
            mBackend.destroySound(status.instance);
            toBeDeletedStatuses.insert(entity);
          });

      for (auto entity : toBeDeletedStatuses) {
        entityDatabase.deleteComponent<AudioStatusComponent>(entity);
      }
    }

    {
      LIQUID_PROFILE_EVENT("Start audios");
      entityDatabase.iterateEntities<AudioSourceComponent, AudioStartComponent>(
          [this, &entityDatabase](auto entity, const auto &source,
                                  const auto &play) {
            if (entityDatabase.hasComponent<AudioStatusComponent>(entity)) {
              return;
            }
            const auto &asset =
                mAssetRegistry.getAudios().getAsset(source.source).data;
            void *sound = mBackend.playSound(asset);

            entityDatabase.setComponent<AudioStatusComponent>(entity, {sound});
          });

      entityDatabase.destroyComponents<liquid::AudioStartComponent>();
    }

    {
      LIQUID_PROFILE_EVENT(
          "Delete audio status components for finished audios");

      std::set<Entity> toBeDeletedStatuses;
      entityDatabase.iterateEntities<AudioStatusComponent>(
          [&toBeDeletedStatuses, this](auto entity, const auto &status) {
            if (!mBackend.isPlaying(status.instance)) {
              mBackend.destroySound(status.instance);
              toBeDeletedStatuses.insert(entity);
            }
          });

      for (auto entity : toBeDeletedStatuses) {
        entityDatabase.deleteComponent<AudioStatusComponent>(entity);
      }
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

    entityDatabase.destroyComponents<AudioStatusComponent>();
    entityDatabase.destroyComponents<AudioStartComponent>();
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
