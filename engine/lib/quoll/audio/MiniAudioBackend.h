#pragma once

#include "AudioAsset.h"

namespace quoll {

class MiniAudioBackend : NoCopyMove {
  class BackendImpl;

public:
  MiniAudioBackend();

  ~MiniAudioBackend();

  void *playSound(const AudioAsset &asset);

  void destroySound(void *instance);

  bool isPlaying(void *instance);

private:
  BackendImpl *mImpl = nullptr;
};

} // namespace quoll
