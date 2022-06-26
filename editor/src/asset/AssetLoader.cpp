#include "liquid/core/Base.h"
#include "AssetLoader.h"
#include "GLTFImporter.h"

namespace liquidator {

const std::vector<liquid::String> AssetLoader::ScriptExtensions{"lua"};
const std::vector<liquid::String> AssetLoader::AudioExtensions{"wav", "mp3"};
const std::vector<liquid::String> AssetLoader::SceneExtensions{"gltf"};

/**
 * @brief Get unique path for a given path
 *
 * Appends incrementing number at the end of
 * file to until the file no longer exists in
 * directory
 *
 * @param path File path
 * @return Unique file path
 */
static liquid::Path getUniquePath(liquid::Path path) {
  auto ext = path.extension();

  auto tmpPath = path;

  uint32_t index = 1;
  while (std::filesystem::exists(tmpPath)) {
    liquid::String uniqueSuffix = "-" + std::to_string(index++);
    tmpPath = path;
    tmpPath.replace_extension();
    tmpPath += uniqueSuffix;
    tmpPath.replace_extension(ext);
  }

  return tmpPath;
}

AssetLoader::AssetLoader(liquid::AssetManager &assetManager,
                         liquid::rhi::ResourceRegistry &resourceRegistry)
    : mAssetManager(assetManager), mDeviceRegistry(resourceRegistry) {}

liquid::Result<bool> AssetLoader::loadFromPath(const liquid::Path &path,
                                               const liquid::Path &directory) {
  auto ext = path.extension().string();
  ext.erase(0, 1);

  auto isExtension = [&ext](const auto &extensions) {
    return std::find(extensions.begin(), extensions.end(), ext) !=
           extensions.end();
  };

  if (isExtension(SceneExtensions)) {
    auto res = GLTFImporter(mAssetManager).loadFromFile(path, directory);
    mAssetManager.getRegistry().syncWithDeviceRegistry(mDeviceRegistry);
    return res;
  }

  auto targetPath = getUniquePath(directory / path.filename());

  if (isExtension(ScriptExtensions) || isExtension(AudioExtensions)) {
    if (std::filesystem::copy_file(path, targetPath)) {
      return mAssetManager.loadAsset(targetPath);
    }
  }

  return liquid::Result<bool>::Error("Loaded file is not supported");
}

liquid::Result<bool>
AssetLoader::loadFromFileDialog(const liquid::Path &directory) {

  using FileTypeEntry = liquid::platform_tools::NativeFileDialog::FileTypeEntry;

  std::vector<FileTypeEntry> entries{
      FileTypeEntry{"Scene files", SceneExtensions},
      FileTypeEntry{"Audio files", AudioExtensions},
      FileTypeEntry{"Script files", ScriptExtensions}};

  auto filePath = mNativeFileDialog.getFilePathFromDialog(entries);
  if (filePath.empty())
    return liquid::Result<bool>::Ok(true, {});

  return loadFromPath(filePath, directory);
}

} // namespace liquidator
