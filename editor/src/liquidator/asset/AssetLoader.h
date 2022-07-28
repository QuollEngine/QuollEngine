#pragma once

#include "liquid/asset/AssetManager.h"
#include "liquid/platform-tools/NativeFileDialog.h"

namespace liquidator {

/**
 * @brief Asset loader
 *
 * Loads all supported asset types
 * from the editor
 */
class AssetLoader {
  static const std::vector<liquid::String> ScriptExtensions;
  static const std::vector<liquid::String> AudioExtensions;
  static const std::vector<liquid::String> SceneExtensions;
  static const std::vector<liquid::String> FontExtensions;

public:
  /**
   * @brief Create asset loader
   *
   * @param assetManager Asset manager
   * @param resourceRegistry Resource registry
   */
  AssetLoader(liquid::AssetManager &assetManager,
              liquid::rhi::ResourceRegistry &resourceRegistry);

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
  liquid::AssetManager &mAssetManager;
  liquid::rhi::ResourceRegistry &mDeviceRegistry;
  liquid::platform_tools::NativeFileDialog mNativeFileDialog;
};

} // namespace liquidator
