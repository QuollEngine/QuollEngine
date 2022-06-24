#include "liquid/core/Base.h"
#include "liquid/audio/MiniAudio.h"
#include "AssetManager.h"

namespace liquid {

Result<AudioAssetHandle> AssetManager::loadAudioFromFile(const Path &filePath) {
  auto *decoder = new ma_decoder;

  auto res = ma_decoder_init_file(filePath.string().c_str(), nullptr, decoder);

  if (res != MA_SUCCESS) {
    return Result<AudioAssetHandle>::Error("Cannot load audio file: " +
                                           filePath.string());
  }

  AssetData<AudioAsset> asset;
  asset.path = filePath;
  asset.relativePath = std::filesystem::relative(filePath, mAssetsPath);
  asset.name = asset.relativePath.string();
  asset.type = AssetType::Audio;
  asset.data.data = decoder;

  return Result<AudioAssetHandle>::Ok(mRegistry.getAudios().addAsset(asset));
}

} // namespace liquid
