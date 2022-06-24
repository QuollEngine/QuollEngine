#include "liquid/core/Base.h"
#include "liquid/audio/AudioSystem.h"

#include "MiniAudio.h"

namespace liquid {

/**
 * @brief Audio system implementation
 *
 * Based on MiniAudio
 */
class AudioSystem::AudioSystemImpl {
public:
  /**
   * @brief Create miniaudio engine
   */
  AudioSystemImpl() {
    auto result = ma_engine_init(nullptr, &mEngine);
    LIQUID_ASSERT(result == MA_SUCCESS, "Could not create audio engine");
  }

  /**
   * @brief Destroy miniaudio engine
   */
  ~AudioSystemImpl() { ma_engine_uninit(&mEngine); }

  AudioSystemImpl(const AudioSystemImpl &) = delete;
  AudioSystemImpl(AudioSystemImpl &&) = delete;
  AudioSystemImpl &operator=(const AudioSystemImpl &) = delete;
  AudioSystemImpl &operator=(AudioSystemImpl &&) = delete;

  /**
   * @brief Play sound
   *
   * @param handle Audio asset handle
   * @param assetRegistry Asset registry
   */
  void playSound(AudioAssetHandle handle, AssetRegistry &assetRegistry) {
    auto *sound = new ma_sound;

    ma_decoder *data = static_cast<ma_decoder *>(
        assetRegistry.getAudios().getAsset(handle).data.data);

    auto res =
        ma_sound_init_from_data_source(&mEngine, data, 0, nullptr, sound);
    LIQUID_ASSERT(res == MA_SUCCESS, "Cannot init sound");

    ma_sound_start(sound);
  }

private:
  ma_engine mEngine;
};

AudioSystem::AudioSystem(AssetRegistry &assetRegistry)
    : mAssetRegistry(assetRegistry), mImpl(new AudioSystemImpl) {}

AudioSystem::~AudioSystem() { delete mImpl; }

void AudioSystem::playSound(AudioAssetHandle handle) {
  mImpl->playSound(handle, mAssetRegistry);
}

} // namespace liquid
