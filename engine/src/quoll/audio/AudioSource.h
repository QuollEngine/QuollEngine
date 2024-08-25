#pragma once

#include "quoll/asset/AssetHandle.h"
#include "AudioAsset.h"

namespace quoll {

struct AudioSource {
  AssetHandle<AudioAsset> source;
};

} // namespace quoll
