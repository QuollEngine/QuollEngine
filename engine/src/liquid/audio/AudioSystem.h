#pragma once

#include "liquid/asset/AssetRegistry.h"
#include "liquid/entity/EntityDatabase.h"

#include "liquid/audio/DefaultAudioBackend.h"

namespace liquid {

/**
 * @brief Audio system
 *
 * @tparam AudioBackend Audio backend
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
      entityDatabase.iterateEntities<AudioStart, Delete>(
          [this, &toBeDeletedStarts](auto entity, auto &status, const auto &) {
            toBeDeletedStarts.insert(entity);
          });

      for (auto entity : toBeDeletedStarts) {
        entityDatabase.remove<AudioStart>(entity);
      }
    }

    {
      LIQUID_PROFILE_EVENT("Cleanup audio statuses for deleted components");

      std::set<Entity> toBeDeletedStatuses;
      entityDatabase.iterateEntities<AudioStatus, Delete>(
          [this, &toBeDeletedStatuses](auto entity, auto &status,
                                       const auto &) {
            mBackend.destroySound(status.instance);
            toBeDeletedStatuses.insert(entity);
          });

      for (auto entity : toBeDeletedStatuses) {
        entityDatabase.remove<AudioStatus>(entity);
      }
    }

    {
      LIQUID_PROFILE_EVENT("Start audios");
      entityDatabase.iterateEntities<AudioSource, AudioStart>(
          [this, &entityDatabase](auto entity, const auto &source,
                                  const auto &play) {
            if (entityDatabase.has<AudioStatus>(entity)) {
              return;
            }
            const auto &asset =
                mAssetRegistry.getAudios().getAsset(source.source).data;
            void *sound = mBackend.playSound(asset);

            entityDatabase.set<AudioStatus>(entity, {sound});
          });

      entityDatabase.destroyComponents<liquid::AudioStart>();
    }

    {
      LIQUID_PROFILE_EVENT(
          "Delete audio status components for finished audios");

      std::set<Entity> toBeDeletedStatuses;
      entityDatabase.iterateEntities<AudioStatus>(
          [&toBeDeletedStatuses, this](auto entity, const auto &status) {
            if (!mBackend.isPlaying(status.instance)) {
              mBackend.destroySound(status.instance);
              toBeDeletedStatuses.insert(entity);
            }
          });

      for (auto entity : toBeDeletedStatuses) {
        entityDatabase.remove<AudioStatus>(entity);
      }
    }
  }

  /**
   * @brief Cleanup all the audio instances
   *
   * @param entityDatabase Entity database
   */
  void cleanup(EntityDatabase &entityDatabase) {
    entityDatabase.iterateEntities<AudioStatus>(
        [this](auto entity, const auto &status) {
          mBackend.destroySound(status.instance);
        });

    entityDatabase.destroyComponents<AudioStatus>();
    entityDatabase.destroyComponents<AudioStart>();
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
