#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "AudioBackend.h"

namespace quoll {

struct SystemView;
class MiniAudioBackend;

class AudioSystem {
public:
  AudioSystem(AudioBackend *backend, AssetRegistry &assetRegistry);

  void createSystemViewData(SystemView &view);

  void output(SystemView &view);

  void cleanup(SystemView &view);

  inline AudioBackend *getBackend() { return mBackend.get(); }

private:
  std::unique_ptr<AudioBackend> mBackend;
  AssetRegistry &mAssetRegistry;
};

} // namespace quoll
