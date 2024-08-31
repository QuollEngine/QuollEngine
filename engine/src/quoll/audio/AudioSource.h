#pragma once

#include "quoll/asset/AssetRef.h"
#include "AudioAsset.h"

namespace quoll {

struct AudioSource {
  AssetRef<AudioAsset> source;
};

} // namespace quoll
