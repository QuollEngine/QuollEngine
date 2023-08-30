#include "quoll/core/Base.h"
#include "MiniAudio.h"
#include "MiniAudioBackend.h"

namespace quoll {

/**
 * @brief Miniaudio sound interface
 */
struct MiniAudioSoundInstance {
  /**
   * @brief Miniaudio decoder
   */
  ma_decoder decoder;

  /**
   * @brief Miniaudio sound
   */
  ma_sound sound;
};

/**
 * @brief Miniaudio backend implementation
 *
 * Based on miniaudio
 */
class MiniAudioBackend::BackendImpl {
public:
  /**
   * @brief Create miniaudio engine
   */
  BackendImpl() {
    auto result = ma_engine_init(nullptr, &mEngine);
    LIQUID_ASSERT(result == MA_SUCCESS, "Could not create audio engine");
  }

  /**
   * @brief Destroy miniaudio engine
   */
  ~BackendImpl() { ma_engine_uninit(&mEngine); }

  BackendImpl(const BackendImpl &) = delete;
  BackendImpl(BackendImpl &&) = delete;
  BackendImpl &operator=(const BackendImpl &) = delete;
  BackendImpl &operator=(BackendImpl &&) = delete;

  /**
   * @brief Get encoding format
   *
   * @param format Asset format
   * @return Encoding format
   */
  ma_encoding_format getEncodingFormat(AudioAssetFormat format) {
    switch (format) {
    case AudioAssetFormat::Wav:
      return ma_encoding_format_wav;
    case AudioAssetFormat::Unknown:
    default:
      return ma_encoding_format_unknown;
    }
  }

  /**
   * @brief Play sound
   *
   * @param asset Audio asset data
   * @return MiniAudio sound instance
   */
  void *playSound(const AudioAsset &asset) {
    auto *instance = new MiniAudioSoundInstance;

    {
      auto config = ma_decoder_config_init_default();
      config.encodingFormat = getEncodingFormat(asset.format);
      auto res = ma_decoder_init_memory(asset.bytes.data(), asset.bytes.size(),
                                        &config, &instance->decoder);
      LIQUID_ASSERT(res == MA_SUCCESS,
                    "Cannot initialize MiniAudio data source");
    }

    {
      auto res = ma_sound_init_from_data_source(&mEngine, &instance->decoder, 0,
                                                nullptr, &instance->sound);
      LIQUID_ASSERT(res == MA_SUCCESS, "Cannot initialize MiniAudio sound");
    }

    ma_sound_start(&instance->sound);

    return instance;
  }

  /**
   * @brief Destroy sound
   *
   * @param instance MiniAudio sound instance
   */
  void destroySound(MiniAudioSoundInstance *instance) {
    ma_sound_uninit(&instance->sound);
    ma_decoder_uninit(&instance->decoder);
    delete instance;
  }

  /**
   * @brief Check if sound is playing
   *
   * @param instance MiniAudio sound instance
   * @retval true Sound is playing
   * @retval false Sound is not playing
   */
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
