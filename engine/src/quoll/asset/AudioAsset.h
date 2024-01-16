#pragma once

namespace quoll {

enum class AudioAssetFormat { Unknown = 0, Wav };

struct AudioAsset {
  std::vector<char> bytes{};

  AudioAssetFormat format = AudioAssetFormat::Wav;
};

} // namespace quoll
