#pragma once

#include "liquid/asset/AssetRegistry.h"

namespace liquid {

/**
 * @brief Audio system
 */
class AudioSystem {
  /**
   * @brief Audio system implementation
   *
   * Based on MiniAudio
   */
  class AudioSystemImpl;

public:
  /**
   * @brief Create audio system
   *
   * @param assetRegistry Asset regsitry
   */
  AudioSystem(AssetRegistry &assetRegistry);

  /**
   * @brief Destroy audio system
   */
  ~AudioSystem();

  /**
   * @brief Play sound
   *
   * @param handle Audio asset handle
   */
  void playSound(AudioAssetHandle handle);

private:
  AudioSystemImpl *mImpl = nullptr;
  AssetRegistry &mAssetRegistry;
};

} // namespace liquid
