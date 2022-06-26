#pragma once

namespace liquid {

/**
 * @brief MiniAudio backend
 */
class MiniAudioBackend {
  class BackendImpl;

public:
  /**
   * @brief Create miniaudio engine
   */
  MiniAudioBackend();

  /**
   * @brief Destroy miniaudio engine
   */
  ~MiniAudioBackend();

  MiniAudioBackend(const MiniAudioBackend &) = delete;
  MiniAudioBackend(MiniAudioBackend &&) = delete;
  MiniAudioBackend &operator=(const MiniAudioBackend &) = delete;
  MiniAudioBackend &operator=(MiniAudioBackend &&) = delete;

  /**
   * @brief Play sound
   *
   * @param data Audio data
   * @return Sound object
   */
  void *playSound(void *data);

  /**
   * @brief Destroy sound
   *
   * @param sound
   */
  void destroySound(void *sound);

  /**
   * @brief Check if sound is playing
   *
   * @param sound Sound
   * @retval true Sound is playing
   * @retval false Sound is not playing
   */
  bool isPlaying(void *sound);

private:
  BackendImpl *mImpl = nullptr;
};

} // namespace liquid
