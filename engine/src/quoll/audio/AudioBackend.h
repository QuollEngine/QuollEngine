#pragma once

#include "quoll/asset/AudioAsset.h"

namespace quoll {

class AudioBackend : public NoCopyMove {
public:
  virtual ~AudioBackend() = default;

  virtual void *playSound(const AudioAsset &asset) = 0;

  virtual void destroySound(void *instance) = 0;

  virtual bool isPlaying(void *instance) = 0;
};

} // namespace quoll