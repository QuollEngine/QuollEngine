#pragma once

#include "AssetManager.h"
#include "liquid/platform-tools/NativeFileDialog.h"

namespace liquid::editor {

/**
 * @brief Asset loader
 *
 * Loads all supported asset types
 * from the editor
 */
class AssetLoader {
public:
  /**
   * @brief Create asset loader
   *
   * @param assetManager Asset manager
   * @param renderStorage Render storage
   */
  AssetLoader(AssetManager &assetManager, RenderStorage &renderStorage);

  /**
   * @brief Load asset from path
   *
   * @param path Path to asset
   * @param directory Target directory path
   * @return Asset load result
   */
  Result<bool> loadFromPath(const Path &path, const Path &directory);

  /**
   * @brief Load asset from native file dialog
   *
   * @param directory Target directory path
   * @return Asset load result
   */
  Result<bool> loadFromFileDialog(const Path &directory);

private:
  AssetManager &mAssetManager;
  platform_tools::NativeFileDialog mNativeFileDialog;

  RenderStorage &mRenderStorage;
};

} // namespace liquid::editor
