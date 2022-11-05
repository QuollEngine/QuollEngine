#include "liquid/core/Base.h"
#include "AssetLoader.h"
#include "GLTFImporter.h"

namespace liquidator {

AssetLoader::AssetLoader(AssetManager &assetManager,
                         liquid::rhi::RenderDevice *device)
    : mAssetManager(assetManager), mDevice(device) {}

liquid::Result<bool> AssetLoader::loadFromPath(const liquid::Path &path,
                                               const liquid::Path &directory) {
  auto res = mAssetManager.importAsset(path, directory);

  if (res.hasData()) {
    mAssetManager.getAssetsCache().getRegistry().syncWithDevice(mDevice);
  }

  return res;
}

liquid::Result<bool>
AssetLoader::loadFromFileDialog(const liquid::Path &directory) {
  using FileTypeEntry = liquid::platform_tools::NativeFileDialog::FileTypeEntry;

  std::vector<FileTypeEntry> entries{
      FileTypeEntry{"Scene files", AssetManager::SceneExtensions},
      FileTypeEntry{"Audio files", AssetManager::AudioExtensions},
      FileTypeEntry{"Script files", AssetManager::ScriptExtensions},
      FileTypeEntry{"Font files", AssetManager::FontExtensions},
      FileTypeEntry{"Texture files", AssetManager::TextureExtensions}};

  auto filePath = mNativeFileDialog.getFilePathFromDialog(entries);
  if (filePath.empty())
    return liquid::Result<bool>::Ok(true, {});

  return loadFromPath(filePath, directory);
}

} // namespace liquidator
