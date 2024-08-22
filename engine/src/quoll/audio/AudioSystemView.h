#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "AudioSource.h"
#include "AudioStatus.h"

namespace quoll {

struct AudioSystemView {
  EntityDatabaseObserver<AudioSource> audioSourceRemoveObserver;
  EntityDatabaseObserver<AudioStatus> audioStatusRemoveObserver;
};

} // namespace quoll
