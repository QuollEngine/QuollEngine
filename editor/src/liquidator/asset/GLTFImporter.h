#pragma once

#include "liquid/asset/Result.h"
#include "liquid/asset/AssetManager.h"

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
   */
  GLTFImporter(liquid::AssetManager &assetManager);

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
};

} // namespace liquidator
