#pragma once

#include "quoll/core/Profiler.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/EntityDatabase.h"
#include "AudioSource.h"
#include "AudioStart.h"
#include "AudioStatus.h"
#include "DefaultAudioBackend.h"

namespace quoll {

template <class AudioBackend = DefaultAudioBackend> class AudioSystem {
public:
  AudioSystem(AssetRegistry &assetRegistry) : mAssetRegistry(assetRegistry) {}

  void output(EntityDatabase &entityDatabase) {
    QUOLL_PROFILE_EVENT("AudioSystem::output");

    {
      QUOLL_PROFILE_EVENT("Cleanup audio data for delete components");

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
      QUOLL_PROFILE_EVENT("Start audios");
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
      QUOLL_PROFILE_EVENT("Delete audio status components for finished audios");

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

  void cleanup(EntityDatabase &entityDatabase) {
    for (auto [entity, status] : entityDatabase.view<AudioStatus>()) {
      mBackend.destroySound(status.instance);
    }

    entityDatabase.destroyComponents<AudioStatus>();
    entityDatabase.destroyComponents<AudioStart>();
  }

  void observeChanges(EntityDatabase &entityDatabase) {
    mAudioSourceRemoveObserver = entityDatabase.observeRemove<AudioSource>();
    mAudioStatusRemoveObserver = entityDatabase.observeRemove<AudioStatus>();
  }

  inline AudioBackend &getBackend() { return mBackend; }

private:
  AudioBackend mBackend;
  AssetRegistry &mAssetRegistry;

  EntityDatabaseObserver<AudioSource> mAudioSourceRemoveObserver;
  EntityDatabaseObserver<AudioStatus> mAudioStatusRemoveObserver;
};

} // namespace quoll
