#include "quoll/core/Base.h"
#include "MiniAudio.h"
#include "MiniAudioBackend.h"

namespace quoll {

struct MiniAudioSoundInstance {
  ma_decoder decoder;

  ma_sound sound;
};

class MiniAudioBackend::BackendImpl {
public:
  BackendImpl() {
    auto result = ma_engine_init(nullptr, &mEngine);
    QuollAssert(result == MA_SUCCESS, "Could not create audio engine");
  }

  ~BackendImpl() { ma_engine_uninit(&mEngine); }

  BackendImpl(const BackendImpl &) = delete;
  BackendImpl(BackendImpl &&) = delete;
  BackendImpl &operator=(const BackendImpl &) = delete;
  BackendImpl &operator=(BackendImpl &&) = delete;

  ma_encoding_format getEncodingFormat(AudioAssetFormat format) {
    switch (format) {
    case AudioAssetFormat::Wav:
      return ma_encoding_format_wav;
    case AudioAssetFormat::Unknown:
    default:
      return ma_encoding_format_unknown;
    }
  }

  void *playSound(const AudioAsset &asset) {
    auto *instance = new MiniAudioSoundInstance;

    {
      auto config = ma_decoder_config_init_default();
      config.encodingFormat = getEncodingFormat(asset.format);
      auto res = ma_decoder_init_memory(asset.bytes.data(), asset.bytes.size(),
                                        &config, &instance->decoder);
      QuollAssert(res == MA_SUCCESS, "Cannot initialize MiniAudio data source");
    }

    {
      auto res = ma_sound_init_from_data_source(&mEngine, &instance->decoder, 0,
                                                nullptr, &instance->sound);
      QuollAssert(res == MA_SUCCESS, "Cannot initialize MiniAudio sound");
    }

    ma_sound_start(&instance->sound);

    return instance;
  }

  void destroySound(MiniAudioSoundInstance *instance) {
    ma_sound_uninit(&instance->sound);
    ma_decoder_uninit(&instance->decoder);
    delete instance;
  }

  bool isPlaying(MiniAudioSoundInstance *instance) {
    return ma_sound_is_playing(&instance->sound);
  }

private:
  ma_engine mEngine{};
};

MiniAudioBackend::MiniAudioBackend() : mImpl(new BackendImpl) {}

MiniAudioBackend::~MiniAudioBackend() {
  delete mImpl;
  mImpl = nullptr;
}

void *MiniAudioBackend::playSound(const AudioAsset &asset) {
  return mImpl->playSound(asset);
}

void MiniAudioBackend::destroySound(void *instance) {
  mImpl->destroySound(static_cast<MiniAudioSoundInstance *>(instance));
}

bool MiniAudioBackend::isPlaying(void *instance) {
  return mImpl->isPlaying(static_cast<MiniAudioSoundInstance *>(instance));
}

} // namespace quoll
