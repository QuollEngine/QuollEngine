#pragma once

#include "quoll/core/Profiler.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/system/SystemView.h"
#include "AudioSource.h"
#include "AudioStart.h"
#include "AudioStatus.h"
#include "DefaultAudioBackend.h"

namespace quoll {

template <class AudioBackend = DefaultAudioBackend> class AudioSystem {
public:
  void createSystemViewData(SystemView &view) {
    view.audio.audioSourceRemoveObserver =
        view.scene->entityDatabase.observeRemove<AudioSource>();
    view.audio.audioStatusRemoveObserver =
        view.scene->entityDatabase.observeRemove<AudioStatus>();
  }

  void output(SystemView &view) {
    QUOLL_PROFILE_EVENT("AudioSystem::output");
    auto &entityDatabase = view.scene->entityDatabase;

    {
      QUOLL_PROFILE_EVENT("Cleanup audio data for delete components");

      for (auto [entity, source] : view.audio.audioSourceRemoveObserver) {
        if (entityDatabase.has<AudioStart>(entity)) {
          entityDatabase.remove<AudioStart>(entity);
        }

        if (entityDatabase.has<AudioStatus>(entity)) {
          entityDatabase.remove<AudioStatus>(entity);
        }
      }

      for (auto [entity, status] : view.audio.audioStatusRemoveObserver) {
        mBackend.destroySound(status.instance);
      }

      view.audio.audioSourceRemoveObserver.clear();
      view.audio.audioStatusRemoveObserver.clear();
    }

    {
      QUOLL_PROFILE_EVENT("Start audios");
      for (auto [entity, source, play] :
           entityDatabase.view<AudioSource, AudioStart>()) {
        if (!source.asset) {
          continue;
        }

        if (entityDatabase.has<AudioStatus>(entity)) {
          continue;
        }

        void *sound = mBackend.playSound(source.asset.get());

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

  void cleanup(SystemView &view) {
    auto &entityDatabase = view.scene->entityDatabase;
    for (auto [entity, status] : entityDatabase.view<AudioStatus>()) {
      mBackend.destroySound(status.instance);
    }

    entityDatabase.destroyComponents<AudioStatus>();
    entityDatabase.destroyComponents<AudioStart>();
  }

  inline AudioBackend &getBackend() { return mBackend; }

private:
  AudioBackend mBackend;
};

} // namespace quoll
