#pragma once

namespace liquid {

enum class AudioAssetFormat { Unknown = 0, Wav };

/**
 * @brief Audio asset data
 */
struct AudioAsset {
  /**
   * Audio data
   */
  std::vector<char> bytes{};

  /**
   * Audio asset format
   */
  AudioAssetFormat format = AudioAssetFormat::Wav;
};

} // namespace liquid
