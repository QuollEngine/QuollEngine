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
   * @param originalAssetPath Original asset path
   * @param engineAssetPath Engine asset path
   * @return Path to newly created prefab
   */
  liquid::Result<liquid::Path>
  loadFromPath(const liquid::Path &originalAssetPath,
               const liquid::Path &engineAssetPath);

  /**
   * @brief Save binary from ASCII gltf
   *
   * @param source ASCII source path
   * @param destination Destination path
   * @return GLB file path
   */
  liquid::Result<liquid::Path> saveBinary(const liquid::Path &source,
                                          const liquid::Path &destination);

private:
  liquid::AssetManager &mAssetManager;
};

} // namespace liquidator
