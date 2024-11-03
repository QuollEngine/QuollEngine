#include "quoll/core/Base.h"
#include "quoll/platform/tools/FileDialog.h"
#include "AssetLoader.h"
#include "AssetManager.h"
#include "GLTFImporter.h"

namespace quoll::editor {

AssetLoader::AssetLoader(AssetManager &assetManager)
    : mAssetManager(assetManager) {}

Result<Path> AssetLoader::loadFromPath(const Path &path,
                                       const Path &directory) {
  return mAssetManager.importAsset(path, directory);
}

Result<void> AssetLoader::loadFromFileDialog(const Path &directory) {
  using FileTypeEntry = platform::FileDialog::FileTypeEntry;

  const std::vector<FileTypeEntry> entries{
      FileTypeEntry{"Prefab files", AssetManager::PrefabExtensions},
      FileTypeEntry{"Audio files", AssetManager::AudioExtensions},
      FileTypeEntry{"Lua script files", AssetManager::ScriptExtensions},
      FileTypeEntry{"Font files", AssetManager::FontExtensions},
      FileTypeEntry{"Texture files", AssetManager::TextureExtensions},
      FileTypeEntry{"Animator files", AssetManager::AnimatorExtensions},
      FileTypeEntry{"Input map files", AssetManager::InputMapExtensions},
      FileTypeEntry{"Environment files", AssetManager::EnvironmentExtensions}};

  auto filePath = platform::FileDialog::getFilePathFromDialog(entries);
  if (filePath.empty())
    return Ok();

  auto res = loadFromPath(filePath, directory);

  if (!res) {
    return res.error();
  }
  return {res.warnings()};
}

} // namespace quoll::editor
