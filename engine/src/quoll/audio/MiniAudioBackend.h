#pragma once

#include "AudioAsset.h"
#include "AudioBackend.h"

namespace quoll {

class MiniAudioBackend : public AudioBackend {
  class BackendImpl;

public:
  MiniAudioBackend();

  ~MiniAudioBackend();

  void *playSound(const AudioAsset &asset) override;

  void destroySound(void *instance) override;

  bool isPlaying(void *instance) override;

private:
  BackendImpl *mImpl = nullptr;
};

} // namespace quoll
