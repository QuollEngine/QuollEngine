#pragma once

#include "liquid/asset/Result.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/rhi/ResourceRegistry.h"

namespace liquidator {

/**
 * @brief GLTF importer
 *
 * Imports GLTF into asset registry
 */
class GLTFImporter {
public:
  /**
   * @brief Create GLTF importer
   *
   * @param assetManager Asset manager
   * @param deviceRegistry Device registry
   */
  GLTFImporter(liquid::AssetManager &assetManager,
               liquid::rhi::ResourceRegistry &deviceRegistry);

  /**
   * @brief Load GLTF from file
   *
   * @param filePath File path
   * @param directory Destination path
   * @return Load result
   */
  liquid::Result<bool> loadFromFile(const liquid::Path &filePath,
                                    const std::filesystem::path &directory);

private:
  liquid::AssetManager &mAssetManager;
  liquid::rhi::ResourceRegistry &mDeviceRegistry;
};

} // namespace liquidator
