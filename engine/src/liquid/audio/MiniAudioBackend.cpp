#include "liquid/core/Base.h"
#include "MiniAudio.h"
#include "MiniAudioBackend.h"

namespace liquid {

/**
 * @brief Miniaudio backend implementation
 *
 * Based on MiniAudio
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
   * @brief Play sound
   *
   * @param dataSource Audio data source
   * @return Sound object
   */
  void *playSound(void *dataSource) {
    auto *sound = new ma_sound;

    auto res =
        ma_sound_init_from_data_source(&mEngine, dataSource, 0, nullptr, sound);
    LIQUID_ASSERT(res == MA_SUCCESS, "Cannot init sound");

    ma_sound_start(sound);

    return sound;
  }

  /**
   * @brief Destroy sound
   *
   * @param sound Sound object
   */
  void destroySound(void *sound) {
    ma_sound_uninit(static_cast<ma_sound *>(sound));
    delete sound;
  }

  /**
   * @brief Check if sound is playing
   *
   * @param sound Sound object
   * @retval true Sound is playing
   * @retval false Sound is not playing
   */
  bool isPlaying(void *sound) {
    return ma_sound_is_playing(static_cast<const ma_sound *>(sound));
  }

private:
  ma_engine mEngine{};
};

MiniAudioBackend::MiniAudioBackend() : mImpl(new BackendImpl) {}

MiniAudioBackend::~MiniAudioBackend() {
  delete mImpl;
  mImpl = nullptr;
}

void *MiniAudioBackend::playSound(void *data) { return mImpl->playSound(data); }

void MiniAudioBackend::destroySound(void *sound) {
  return mImpl->destroySound(sound);
}

bool MiniAudioBackend::isPlaying(void *sound) {
  return mImpl->isPlaying(sound);
}

} // namespace liquid
