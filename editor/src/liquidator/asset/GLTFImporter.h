#pragma once

#include "liquid/asset/Result.h"
#include "gltf/GLTFImportData.h"

namespace liquid::editor {

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
   * @param assetCache Asset cache
   */
  GLTFImporter(AssetCache &assetCache);

  /**
   * @brief Load GLTF from file
   *
   * @param originalAssetPath Original asset path
   * @param engineAssetPath Engine asset path
   * @return Path to newly created prefab
   */
  Result<Path> loadFromPath(const Path &originalAssetPath,
                            const Path &engineAssetPath);

  /**
   * @brief Save binary from ASCII gltf
   *
   * @param source ASCII source path
   * @param destination Destination path
   * @return GLB file path
   */
  Result<Path> saveBinary(const Path &source, const Path &destination);

private:
  AssetCache &mAssetCache;
};

} // namespace liquid::editor
