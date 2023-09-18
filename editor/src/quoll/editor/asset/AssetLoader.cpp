#include "quoll/core/Base.h"
#include "quoll/platform/tools/FileDialog.h"
#include "AssetLoader.h"
#include "GLTFImporter.h"

namespace quoll::editor {

AssetLoader::AssetLoader(AssetManager &assetManager)
    : mAssetManager(assetManager) {}

Result<Path> AssetLoader::loadFromPath(const Path &path,
                                       const Path &directory) {
  auto res = mAssetManager.importAsset(path, directory);

  if (res.hasData()) {
    mAssetManager.getAssetRegistry().syncWithDevice(
        mAssetManager.getRenderStorage());
    mAssetManager.generatePreview(res.getData(),
                                  mAssetManager.getRenderStorage());
  }

  return res;
}

Result<bool> AssetLoader::loadFromFileDialog(const Path &directory) {
  using FileTypeEntry = platform::FileDialog::FileTypeEntry;

  std::vector<FileTypeEntry> entries{
      FileTypeEntry{"Prefab files", AssetManager::PrefabExtensions},
      FileTypeEntry{"Audio files", AssetManager::AudioExtensions},
      FileTypeEntry{"Script files", AssetManager::ScriptExtensions},
      FileTypeEntry{"Font files", AssetManager::FontExtensions},
      FileTypeEntry{"Texture files", AssetManager::TextureExtensions},
      FileTypeEntry{"Animator files", AssetManager::AnimatorExtensions},
      FileTypeEntry{"Input map files", AssetManager::InputMapExtensions},
      FileTypeEntry{"Environment files", AssetManager::EnvironmentExtensions}};

  auto filePath = platform::FileDialog::getFilePathFromDialog(entries);
  if (filePath.empty())
    return Result<bool>::Ok(true, {});

  auto res = loadFromPath(filePath, directory);

  if (res.hasError()) {
    return Result<bool>::Error(res.getError());
  }
  return Result<bool>::Ok(true, res.getWarnings());
}

} // namespace quoll::editor
