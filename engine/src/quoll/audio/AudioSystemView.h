#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "AudioSource.h"
#include "AudioStatus.h"

namespace quoll {

struct AudioSystemView {
  flecs::query<AudioSource> queryAudioSources;
  flecs::query<AudioStatus> queryAudioStatuses;
};

} // namespace quoll
