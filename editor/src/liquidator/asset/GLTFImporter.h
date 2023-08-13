#pragma once

#include "liquid/asset/Result.h"
#include "gltf/GLTFImportData.h"
#include "ImageLoader.h"

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
   * @param imageLoader Image loader
   * @param optimize Enable optimizations
   */
  GLTFImporter(AssetCache &assetCache, ImageLoader &imageLoader, bool optimize);

  /**
   * @brief Load GLTF from file
   *
   * @param sourceAssetPath Source asset path
   * @param uuids Existing asset uuids
   * @return Uuids of newly created assets
   */
  Result<UUIDMap> loadFromPath(const Path &sourceAssetPath,
                               const UUIDMap &uuids);

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
  ImageLoader &mImageLoader;
  bool mOptimize = false;
};

} // namespace liquid::editor
