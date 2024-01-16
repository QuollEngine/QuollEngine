#pragma once

#include "quoll/asset/AudioAsset.h"

namespace quoll {

class MiniAudioBackend {
  class BackendImpl;

public:
  MiniAudioBackend();

  ~MiniAudioBackend();

  MiniAudioBackend(const MiniAudioBackend &) = delete;
  MiniAudioBackend(MiniAudioBackend &&) = delete;
  MiniAudioBackend &operator=(const MiniAudioBackend &) = delete;
  MiniAudioBackend &operator=(MiniAudioBackend &&) = delete;

  void *playSound(const AudioAsset &asset);

  void destroySound(void *instance);

  bool isPlaying(void *instance);

private:
  BackendImpl *mImpl = nullptr;
};

} // namespace quoll
