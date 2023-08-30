#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/EntityDatabase.h"

#include "quoll/audio/DefaultAudioBackend.h"

namespace quoll {

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
      LIQUID_PROFILE_EVENT("Cleanup audio data for delete components");

      for (auto [entity, source] : mAudioSourceRemoveObserver) {
        if (entityDatabase.has<AudioStart>(entity)) {
          entityDatabase.remove<AudioStart>(entity);
        }

        if (entityDatabase.has<AudioStatus>(entity)) {
          entityDatabase.remove<AudioStatus>(entity);
        }
      }

      for (auto [entity, status] : mAudioStatusRemoveObserver) {
        mBackend.destroySound(status.instance);
      }

      mAudioSourceRemoveObserver.clear();
      mAudioStatusRemoveObserver.clear();
    }

    {
      LIQUID_PROFILE_EVENT("Start audios");
      for (auto [entity, source, play] :
           entityDatabase.view<AudioSource, AudioStart>()) {
        if (entityDatabase.has<AudioStatus>(entity)) {
          continue;
        }
        const auto &asset =
            mAssetRegistry.getAudios().getAsset(source.source).data;
        void *sound = mBackend.playSound(asset);

        entityDatabase.set<AudioStatus>(entity, {sound});
      }

      entityDatabase.destroyComponents<quoll::AudioStart>();
    }

    {
      LIQUID_PROFILE_EVENT(
          "Delete audio status components for finished audios");

      std::set<Entity> toBeDeletedStatuses;
      for (auto [entity, status] : entityDatabase.view<AudioStatus>()) {
        if (!mBackend.isPlaying(status.instance)) {
          toBeDeletedStatuses.insert(entity);
        }
      }

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
    for (auto [entity, status] : entityDatabase.view<AudioStatus>()) {
      mBackend.destroySound(status.instance);
    }

    entityDatabase.destroyComponents<AudioStatus>();
    entityDatabase.destroyComponents<AudioStart>();
  }

  /**
   * @brief Observer changes in entities
   *
   * @param entityDatabase Entity database
   */
  void observeChanges(EntityDatabase &entityDatabase) {
    mAudioSourceRemoveObserver = entityDatabase.observeRemove<AudioSource>();
    mAudioStatusRemoveObserver = entityDatabase.observeRemove<AudioStatus>();
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

  EntityDatabaseObserver<AudioSource> mAudioSourceRemoveObserver;
  EntityDatabaseObserver<AudioStatus> mAudioStatusRemoveObserver;
};

} // namespace quoll
