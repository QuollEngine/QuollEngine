#include "quoll/core/Base.h"
#include "quoll/core/Profiler.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/system/SystemView.h"
#include "AudioSource.h"
#include "AudioStart.h"
#include "AudioStatus.h"
#include "AudioSystem.h"

namespace quoll {

AudioSystem::AudioSystem(AudioBackend *backend, AssetRegistry &assetRegistry)
    : mBackend(backend), mAssetRegistry(assetRegistry) {}

void AudioSystem::createSystemViewData(SystemView &view) {
  auto &db = view.scene->entityDatabase;
  view.audio.queryAudioSources = db.query_builder<AudioSource>()
                                     .with<AudioStart>()
                                     .without<AudioStatus>()
                                     .build();

  view.audio.queryAudioStatuses = db.query<AudioStatus>();

  db.observer<AudioSource>()
      .event(flecs::OnRemove)
      .each([](flecs::entity entity, AudioSource &) {
        if (entity.has<AudioStart>()) {
          entity.remove<AudioStart>();
        }

        if (entity.has<AudioStatus>()) {
          entity.remove<AudioStatus>();
        }
      });

  db.observer<AudioStatus>()
      .event(flecs::OnRemove)
      .each([this](flecs::entity entity, auto &status) {
        mBackend->destroySound(status.instance);
      });
}

void AudioSystem::output(SystemView &view) {
  QUOLL_PROFILE_EVENT("AudioSystem::output");
  auto &entityDatabase = view.scene->entityDatabase;

  {
    QUOLL_PROFILE_EVENT("Start audios");

    entityDatabase.defer_begin();
    view.audio.queryAudioSources.each(
        [this](flecs::entity entity, AudioSource &source) {
          const auto &asset =
              mAssetRegistry.getAudios().getAsset(source.source).data;
          void *sound = mBackend->playSound(asset);

          entity.set<AudioStatus>({sound});
        });
    entityDatabase.defer_end();

    entityDatabase.remove_all<AudioStart>();
  }

  {
    QUOLL_PROFILE_EVENT("Delete audio status components for finished audios");

    entityDatabase.defer_begin();
    view.audio.queryAudioStatuses.each(
        [this](flecs::entity entity, AudioStatus &status) {
          if (!mBackend->isPlaying(status.instance)) {
            entity.remove<AudioStatus>();
          }
        });
    entityDatabase.defer_end();
  }
}

void AudioSystem::cleanup(SystemView &view) {
  auto &db = view.scene->entityDatabase;

  db.remove_all<AudioStatus>();
  db.remove_all<AudioStart>();
}

} // namespace quoll
