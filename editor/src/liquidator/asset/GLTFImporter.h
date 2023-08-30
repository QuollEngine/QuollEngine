#pragma once

#include "liquid/asset/Result.h"
#include "gltf/GLTFImportData.h"
#include "ImageLoader.h"

namespace quoll::editor {

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
   * @brief Create embedded GLB file
   *
   * @param source Source path
   * @param destination Destination directory
   * @return GLB file path
   */
  static Result<Path> createEmbeddedGlb(const Path &source,
                                        const Path &destination);

private:
  AssetCache &mAssetCache;
  ImageLoader &mImageLoader;
  bool mOptimize = false;
};

} // namespace quoll::editor
