#include "liquid/core/Base.h"
#include "AssetLoader.h"
#include "GLTFImporter.h"

namespace liquid::editor {

AssetLoader::AssetLoader(AssetManager &assetManager,
                         RenderStorage &renderStorage)
    : mAssetManager(assetManager), mRenderStorage(renderStorage) {}

Result<bool> AssetLoader::loadFromPath(const Path &path,
                                       const Path &directory) {
  auto res = mAssetManager.importAsset(path, directory);

  if (res.hasData()) {
    mAssetManager.getAssetRegistry().syncWithDevice(mRenderStorage);
  }

  return res;
}

Result<bool> AssetLoader::loadFromFileDialog(const Path &directory) {
  using FileTypeEntry = platform_tools::NativeFileDialog::FileTypeEntry;

  std::vector<FileTypeEntry> entries{
      FileTypeEntry{"Scene files", AssetManager::SceneExtensions},
      FileTypeEntry{"Audio files", AssetManager::AudioExtensions},
      FileTypeEntry{"Script files", AssetManager::ScriptExtensions},
      FileTypeEntry{"Font files", AssetManager::FontExtensions},
      FileTypeEntry{"Texture files", AssetManager::TextureExtensions},
      FileTypeEntry{"Environment files", AssetManager::EnvironmentExtensions}};

  auto filePath = mNativeFileDialog.getFilePathFromDialog(entries);
  if (filePath.empty())
    return Result<bool>::Ok(true, {});

  return loadFromPath(filePath, directory);
}

} // namespace liquid::editor
