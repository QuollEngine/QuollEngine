#include "quoll/core/Base.h"
#include "quoll/audio/MiniAudio.h"
#include "AssetCache.h"

namespace quoll {

namespace {

AudioAssetFormat getAudioFormatFromExtension(StringView extension) {
  if (extension == "wav") {
    return AudioAssetFormat::Wav;
  }

  return AudioAssetFormat::Unknown;
}

} // namespace

Result<AudioAsset> AssetCache::loadAudio(const Path &path) {
  auto ext = path.extension().string();
  ext.erase(0, 1);

  auto *decoder = new ma_decoder;

  std::ifstream stream(path, std::ios::binary | std::ios::ate);

  if (stream.bad()) {
    return Error("Cannot load audio file: " + path.string());
  }

  const std::ifstream::pos_type pos = stream.tellg();

  if (pos <= 0) {
    return Error("Could not open file: File is empty");
  }

  std::vector<char> bytes(pos);
  stream.seekg(0, std::ios::beg);
  stream.read(&bytes[0], pos);

  AudioAsset asset;
  asset.bytes = bytes;

  return asset;
}

} // namespace quoll
