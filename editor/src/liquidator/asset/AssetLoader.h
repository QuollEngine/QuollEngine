#pragma once

#include "AssetManager.h"
#include "liquid/platform-tools/NativeFileDialog.h"

namespace liquidator {

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
  AssetLoader(AssetManager &assetManager, liquid::RenderStorage &renderStorage);

  /**
   * @brief Load asset from path
   *
   * @param path Path to asset
   * @param directory Target directory path
   * @return Asset load result
   */
  liquid::Result<bool> loadFromPath(const liquid::Path &path,
                                    const liquid::Path &directory);

  /**
   * @brief Load asset from native file dialog
   *
   * @param directory Target directory path
   * @return Asset load result
   */
  liquid::Result<bool> loadFromFileDialog(const liquid::Path &directory);

private:
  AssetManager &mAssetManager;
  liquid::platform_tools::NativeFileDialog mNativeFileDialog;

  liquid::RenderStorage &mRenderStorage;
};

} // namespace liquidator
